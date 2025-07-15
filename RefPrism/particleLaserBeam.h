//===========================================================================
// particleLaserBeam.h		レーザー周辺のパーティクル処理
// 製作者：野村 悠人
//===========================================================================
#pragma once
#include "gameObject.h"
#include "laserBeam.h"


class ParticleLaserBeam : public GameObject
{
private:
	LaserBeam* m_LaserObj{};
	Color m_Color{};

	ID3D11Buffer* m_VertexBuffer{};
	ID3D11ShaderResourceView* m_Texture{};

	ID3D11InputLayout* m_VertexLayout{};
	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader* m_PixelShader{};

	struct PARTICLE
	{
		Vector3 Pos{};
		Vector3 Vel{};
		Vector3 Acce{};	// 加速度

		int Status{};	// 状態
		int Life{};		// 寿命
		bool Enable{};	// 有効か
	};

	static const int PARTICLE_MAX = 50;
	const float PARTICLE_INIT_VEL = 1.0f;
	PARTICLE m_Particle[PARTICLE_MAX]{};

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw()override;

	ID3D11ShaderResourceView* GetTexture() const {
		return m_Texture;
	}

	void SetLaserObject(LaserBeam* obj) {
		m_LaserObj = obj;
	}
	void SetColor(const Color& color) {
		m_Color = color;
	}
	void SetTexture(ID3D11ShaderResourceView* texture) {
		m_Texture = texture;
	}
};
