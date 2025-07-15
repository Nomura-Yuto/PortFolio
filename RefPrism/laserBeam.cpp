//===========================================================================
// laserBeam.cpp			レーザービーム処理
// 製作者：野村 悠人
//===========================================================================
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "modelRenderer.h"
#include "camera.h"
#include "laserBeam.h"
#include "laserCannon.h"
#include "enemy.h"
#include "obstacle.h"
#include "score.h"
#include "audio.h"

void LaserBeam::Init()
{
	m_Pos.y = 0.5f;
	m_StartPos = Vector3(0.0f, 0.5f, 0.0f);
	m_EndPos = Vector3(5.0f, 0.5f, 0.0f);
	m_Scl = Vector3(PARTICLE_MIN_SIZE, PARTICLE_MIN_SIZE, 1.0f);
	m_Size = Vector3(PARTICLE_MAX_SIZE, 1.0f, 1.0f);

	VERTEX_3D vertex[4];

	vertex[0].Position = Vector3(-0.5f, 0.5f, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(0.0f, 0.0f);

	vertex[1].Position = Vector3(0.5f, 0.5f, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(1.0f, 0.0f);

	vertex[2].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(0.0f, 1.0f);

	vertex[3].Position = Vector3(0.5f, -0.5f, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(1.0f, 1.0f);

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

	m_SE = new Audio(this);
	m_SE->Init();
	m_SE->Load("asset\\audio\\laserHit.wav");

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

void LaserBeam::Uninit()
{
	m_SE->Uninit();
	delete m_SE;

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	m_VertexBuffer->Release();
	m_Texture->Release();

}

void LaserBeam::Update()
{
	Scene* scene = Manager::GetScene();

	if(m_State == STATE_NONE) {
		// 終点の座標を取得
		m_EndPos = m_TargetObj->GetPos();
		m_EndPos.y = m_StartPos.y;

		// 始点から終点へのベクトルを取得
		m_Direction = m_EndPos - m_StartPos;

		// ベクトル長を取得
		m_Length = m_Direction.Length();

		// 中心座標の設定
		m_Pos.x = m_StartPos.x + m_EndPos.x * 0.5f;
		m_Pos.z = m_StartPos.z + m_EndPos.z * 0.5f;
	}
	else {
		// 始点の座標取得
		m_StartPos = m_TargetObj->GetPos();
		m_StartPos.y = 0.5f;

		//m_Length = m_PrevtLaser->GetLength();
		m_Length = REFLASER_LENGTH;

		Vector3 frontVec = m_PrevtLaser->GetFrontVec();		// レーザーの進行方向ベクトル取得
		Vector3 normalVec = m_TargetObj->GetFoward();		// 法線ベクトル取得
		Vector3 refVec;

		// Stateに応じたベクトル計算
		// 反射ベクトル
		if (m_State == STATE_REFLECT) {
			refVec = frontVec.Reflect(normalVec);
		}
		// 屈折ベクトル
		else {
			refVec = frontVec.Refract(normalVec);
		}

		// 終点の設定
		m_EndPos = m_TargetObj->GetPos() + (refVec * m_Length);
		m_EndPos.y = m_StartPos.y;

		// 始点から終点へのベクトルを取得
		m_Direction = GetFrontVec();

		Vector3 normal = m_Direction.Normalize();
		m_Pos.x = m_StartPos.x + normal.x * (m_Length * 0.5f);
		m_Pos.z = m_StartPos.z + normal.z * (m_Length * 0.5f);
	}

	// サイズをレーザーの長さに設定
	m_Size.z = m_Length;

	// 始点から終点を結ぶように角度を調整
	m_Rot.y = atan2f(m_Direction.x, m_Direction.z) + XM_PI;

	// 障害物との当たり判定
	auto obstacleList = scene->GetGameObjects<Obstacle>();
	for (auto obstacle : obstacleList) {
		if (m_State != STATE_NONE) {
			if (m_PrevtLaser->OnCollision(obstacle)) {
				m_Length = 0.0f;
				m_Size.z = m_Length;
				break;
			}
		}
		OnCollision(obstacle);
	}

	auto laserCannon = scene->GetGameObject<LaserCannon>();
	if (laserCannon && m_State != STATE_NONE)OnCollision(laserCannon);

	// 敵との当たり判定
	auto enemyList = scene->GetGameObjects<Enemy>();
	for (auto enemy : enemyList) {
		if (OnCollision(enemy)) {
			scene->GetGameObject<Score>()->AddScore(Manager::Rand(10, 50));
			enemy->Destroy();
			m_SE->Play();
		}
	}

	// パーティクルの使用数の計算
	m_ParticleAmount = m_Length / PARTICLE_SPACE;
	if (m_ParticleAmount > PARTICLE_MAX) m_ParticleAmount = PARTICLE_MAX;

	// パーティクル設定
	for (int i = 0; i < PARTICLE_MAX; i++) {
		switch (m_Particle[i].Status) {
		case 0: // 待機状態
			if (!m_Particle[i].Enable) {
				if (i < m_ParticleAmount) {
					m_Particle[i].Enable = true;
					m_Particle[i].Status = 1;
				}
			}

		case 1:	// // 始点から終点を結ぶ状態
			if (m_Particle[i].Enable) {
				if (i >= m_ParticleAmount) {
					m_Particle[i].Enable = false;
					m_Particle[i].Status = 0;
				}
				// 座標設定
				m_Particle[i].Pos = m_StartPos + m_Direction.Normalize() * (PARTICLE_SPACE * i);
			}
			break;
		case 2:
			if (m_Particle[i].Enable) {
				m_Particle[i].Enable = false;
				m_Particle[i].Status = 0;
			}
		}
	}

	UpdateComponent();
}

void LaserBeam::Draw ()
{
	// 入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定 
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	// カメラのビューマトリクス取得
	Camera* camera = Manager::GetScene()->GetGameObject<Camera>();
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

	// テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Zバッファ無効
	Renderer::SetDepthEnable(false);

	// 加算合成有効
	Renderer::SetBlendAddEnable(true);


	m_Color.a = 1.0f;
	m_Scl = Vector3(PARTICLE_MIN_SIZE, PARTICLE_MIN_SIZE, 1.0f);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = m_Color;
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	for (int i = 0; i < m_ParticleAmount; i++) {
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


	m_Color.a = 0.2f;
	m_Scl = Vector3(PARTICLE_MAX_SIZE, PARTICLE_MAX_SIZE, 1.0f);

	// マテリアル設定
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = m_Color;
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	for (int i = 0; i < m_ParticleAmount; i++) {
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

	// 加算合成無効
	Renderer::SetBlendAddEnable(false);

	DrawComponent();
}

//----------------------------------------------------------------------
// 当たり判定関数 OBBと円での判定
// 戻り値	：当たり判定の有無
// 引数		：判定するGameObject
//----------------------------------------------------------------------
bool LaserBeam::OnCollision(const GameObject* obj)
{
	Vector3 size = GetSize() * 0.5f;
	if (size.z == 0.0f) return false;

	float objRadius = obj->GetRadius();

	Vector3 objPos = obj->GetPos();
	objPos.y = GetStartPos().y;

	Vector3 direction;
	direction = GetPos() - objPos;

	// 分離軸
	Vector3 axisX = GetRight();
	Vector3 axisZ = GetFoward();

	// 内積計算
	float dotX = direction.Dot(axisX);
	float dotZ = direction.Dot(axisZ);

	// 当たっているか判定
	if (-size.x - objRadius < dotX && size.x + objRadius > dotX &&
		-size.z - objRadius < dotZ && size.z + objRadius > dotZ) {
		// レーザーの長さとサイズを調整
		m_Length = (m_StartPos - objPos).Length();
		m_Size.z = m_Length;

		// 終点の設定
		Vector3 normal = m_Direction.Normalize();
		m_EndPos = m_StartPos + (normal * m_Length);

		// 中心座標の設定
		m_Pos.x = m_StartPos.x + normal.x * (m_Length * 0.5f);
		m_Pos.z = m_StartPos.z + normal.z * (m_Length * 0.5f);

		return true;
	}

	return false;
}
