//===========================================================================
// particleLaserBeam.cpp	レーザー周辺のパーティクル処理
// 製作者：野村 悠人
//===========================================================================
#include "main.h"
#include "particleLaserBeam.h"
#include "renderer.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"

void ParticleLaserBeam::Init()
{
	m_Scl = Vector3(0.3f, 0.3f, 1.0f);
	m_Color = Color(1.0f, 1.0f, 1.0f, 0.5f);

	VERTEX_3D vertex[4];

	vertex[0].Position	= Vector3(-0.5f, 0.5f, 0.0f);
	vertex[0].Normal	= Vector3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord	= Vector2(0.0f, 0.0f);

	vertex[1].Position	= Vector3(0.5f, 0.5f, 0.0f);
	vertex[1].Normal	= Vector3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord	= Vector2(1.0f, 0.0f);

	vertex[2].Position	= Vector3(-0.5f, -0.5f, 0.0f);
	vertex[2].Normal	= Vector3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord	= Vector2(0.0f, 1.0f);

	vertex[3].Position	= Vector3(0.5f, -0.5f, 0.0f);
	vertex[3].Normal	= Vector3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord	= Vector2(1.0f, 1.0f);

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd{};
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\particleOrigin.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_Texture);
	assert(m_Texture);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
		"shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader,
		"shader\\unlitTexturePS.cso");
}

void ParticleLaserBeam::Uninit()
{
	m_VertexBuffer->Release();
	m_Texture->Release();

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void ParticleLaserBeam::Update()
{
	Vector3 startPos = m_LaserObj->GetStartPos();
	Vector3 endPos = m_LaserObj->GetEndPos();
	Vector3 front = (endPos - startPos).Normalize();
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 right = m_LaserObj->GetRight();

	// パーティクル発射
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		switch (m_Particle[i].Status) {
		case 0: // 待機状態
			if (!m_Particle[i].Enable) {
				m_Particle[i].Enable = true;
				m_Particle[i].Status = 1;
			}
			break;

		case 1:	// 初期設定
			if (m_Particle[i].Enable) {
				m_Particle[i].Status = 2;

				// 初期座標
				float t = Manager::Randf();
				Vector3 randPos = (t * startPos) + ((1 - t) * endPos);

				float angle = Manager::Randf(0.0f, XM_PI * 2.0f);
				Vector3 offset = Vector3(cosf(angle) + sinf(angle), 0.0f, cosf(angle) - sinf(angle)) * 0.3f;

				m_Particle[i].Pos = randPos + offset;

				// 初速度
				float speed = Manager::Randf(0.0f, 2.0f);
				m_Particle[i].Vel = offset.Normalize() * speed;

				// 減衰速度
				m_Particle[i].Acce = offset.Normalize() * -0.01;

				// 寿命設定
				m_Particle[i].Life = Manager::Rand(30, 50);
			}

		case 2: // 移動状態
			if (m_Particle[i].Enable) {
				m_Particle[i].Vel += m_Particle[i].Acce;

				m_Particle[i].Pos += m_Particle[i].Vel * DT;

				// 寿命の減少
				m_Particle[i].Life--;

				// 寿命が0になったら待機状態に
				if (m_Particle[i].Life <= 0) {
					m_Particle[i].Status = 0;
					m_Particle[i].Enable = false;
				}

				break;
			}
		}
	}
}

void ParticleLaserBeam::Draw()
{
	if (m_LaserObj->GetLength() == 0.0f) return;

	// 入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定 
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	// カメラのビューマトリクス取得
	Scene* scene = Manager::GetScene();
	Camera* camera = scene->GetGameObject<Camera>();
	XMMATRIX view = camera->GetViewMatrix();

	// ビューの逆行列
	XMMATRIX invView;
	invView = XMMatrixInverse(nullptr, view); // 逆行列
	invView.r[3].m128_f32[0] = 0.0f;
	invView.r[3].m128_f32[1] = 0.0f;
	invView.r[3].m128_f32[2] = 0.0f;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = m_Color;
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	// テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Zバッファ無効
	Renderer::SetDepthEnable(false);

	// 加算合成有効
	Renderer::SetBlendAddEnable(true);

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable) {
			// ワールドマトリクス設定
			XMMATRIX world, scale, trans;
			scale = XMMatrixScaling(m_Scl.x, m_Scl.y, m_Scl.z);
			trans = XMMatrixTranslation(m_Particle[i].Pos.x, m_Particle[i].Pos.y, m_Particle[i].Pos.z);
			world = scale * invView * trans;
			Renderer::SetWorldMatrix(world);

			// ポリゴン描画
			Renderer::GetDeviceContext()->Draw(4, 0);
		}
	}

	// Zバッファ有効
	Renderer::SetDepthEnable(true);

	// 加算合成有効
	Renderer::SetBlendAddEnable(false);
}
