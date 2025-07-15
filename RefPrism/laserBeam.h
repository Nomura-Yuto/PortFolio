//===========================================================================
// laserBeam.h				レーザービーム処理
// 製作者：野村 悠人
//===========================================================================
#pragma once
#include "gameObject.h"

typedef enum
{
	STATE_NONE,
	STATE_REFLECT,
	STATE_REFRACT,
	STATE_MAX
}LASER_STATE;

class LaserBeam : public GameObject
{
protected:
	Vector3 m_StartPos{};
	Vector3 m_EndPos{};
	Vector3 m_Direction{};
	Color m_Color = { 0.5f, 0.5f, 1.0f, 1.0f };

	float m_Length{};

	LASER_STATE m_State = STATE_NONE;

	GameObject* m_TargetObj{};
	LaserBeam* m_PrevtLaser{};
			
	ID3D11InputLayout*	m_VertexLayout{};
	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*	m_PixelShader{};

	ID3D11Buffer* m_VertexBuffer{};
	ID3D11ShaderResourceView* m_Texture{};

	class Audio* m_SE{};

	const float REFLASER_LENGTH = 15.0f;

	const float PARTICLE_MIN_SIZE = 0.3f;
	const float PARTICLE_MAX_SIZE = 1.5f;
	const float PARTICLE_SPACE = 0.05f;
	static const int PARTICLE_MAX = 1500;

	int m_ParticleAmount = PARTICLE_MAX;

	struct PARTICLE
	{
		XMFLOAT3 Pos{};
		XMFLOAT3 Vel{};
		XMFLOAT3 TargetPos{};

		int Status{};	// 状態
		int Life{};		// 寿命
		bool Enable{};	// 有効か
	};

	PARTICLE m_Particle[PARTICLE_MAX]{};

public:
	virtual void Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw()override;

	bool OnCollision(const GameObject* obj);
	void SwapLaserState() {
		if (m_State == STATE_NONE) return;
		else if (m_State == STATE_REFLECT) m_State = STATE_REFRACT;
		else m_State = STATE_REFLECT;
	}

	void SetStartPos(const Vector3& pos) {
		m_StartPos = pos;
	}
	void SetEndPos(const Vector3& pos) {
		m_EndPos = pos;
	}
	void SetColor(const Color& color) {
		m_Color = color;
	}
	void SetTargetObj(GameObject* obj) {
		m_TargetObj = obj;
	}
	void SetLaserState(LASER_STATE state) {
		m_State = state;
	}
	void SetPrevLaser(LaserBeam* laser) {
		m_PrevtLaser = laser;
	}

	Vector3 GetStartPos() const {
		return m_StartPos;
	}
	Vector3 GetEndPos() const {
		return m_EndPos;
	}
	Vector3 GetFrontVec() const {
		return m_EndPos - m_StartPos;
	}
	float GetLength() const {
		return m_Length;
	}
	Color GetColor() const {
		return m_Color;
	}
	GameObject* GetTargetObj() const {
		return m_TargetObj;
	}
	LASER_STATE GetLaserState() const {
		return m_State;
	}
};
