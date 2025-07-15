//===============================================
//感度設定制御[sensi.cpp]
//class：上位モジュール
//Date:2024/02/02	Auther:嶋田
//===============================================
#include "memleakcheck.h"
#ifdef _DEBUG
#pragma push_macro("new")
#undef new
#endif
#include "planePolygon.h"
#include "camera.h"

#ifdef _DEBUG
#pragma pop_macro("new")
#endif
#include "sensi.h"
#include "input.h"

Sensi::Sensi()
{
	pSensiBG = new SensiBG();
	pSensiBar = new SensiBar();
	pSensiSlider = new SensiSlider();
	pSensiEscape = new SensiEscape();
}

Sensi::~Sensi()
{
	delete pSensiBG;
	pSensiBG = nullptr;
	delete pSensiBar;
	pSensiBar = nullptr;
	delete pSensiSlider;
	pSensiSlider = nullptr;
	delete pSensiEscape;
	pSensiEscape = nullptr;
}

void Sensi::Update(void)
{
	pSensiBG->Update();
	pSensiBar->Update();
	pSensiSlider->Update();
	pSensiEscape->Update();
}

void Sensi::Draw(void)
{
	pSensiBG->Draw();
	pSensiBar->Draw();
	pSensiSlider->Draw();
	pSensiEscape->Draw();
}

// 感度設定背景
SensiBG::SensiBG()
{
	pLoadSensiBG = new LoadSensiBG();
	pLoadSensiBG->LoadPlaneTexture();
	SetPos(POSITION);
	SetSize(SIZE);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

SensiBG::~SensiBG()
{
	delete pLoadSensiBG;
	pLoadSensiBG = nullptr;
}

void SensiBG::Update(void)
{
}

void SensiBG::Draw(void)
{
	DrawSpriteLeftTop(pLoadSensiBG->GetTexture(), GetPos().x, GetPos().y, GetSize().x, GetSize().y,
		pLoadSensiBG->GetU(),  pLoadSensiBG->GetV(),
		pLoadSensiBG->GetUW(), pLoadSensiBG->GetVH(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}

// 感度設定バー
SensiBar::SensiBar()
{
	pLoadSensiBar = new LoadSensiBar();
	pLoadSensiBar->LoadPlaneTexture();
	SetPos(POSITION);
	SetSize(SIZE);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

SensiBar::~SensiBar()
{
	delete pLoadSensiBar;
	pLoadSensiBar = nullptr;
}

void SensiBar::Update(void)
{
}

void SensiBar::Draw(void)
{
	DrawSprite(pLoadSensiBar->GetTexture(), GetPos().x, GetPos().y, GetSize().x, GetSize().y,
		pLoadSensiBar->GetU(), pLoadSensiBar->GetV(),
		pLoadSensiBar->GetUW(), pLoadSensiBar->GetVH(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}

// 感度設定スライダー
SensiSlider::SensiSlider()
{
	pLoadSensiSlider = new LoadSensiSlider();
	pLoadSensiSlider->LoadPlaneTexture();
	SetPos(POSITION);
	SetSize(SIZE);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	m_camera_sensi = GetCameraSensi();
	m_f = 100.0f;
	if (m_camera_sensi == SENS_LOW)
	{
		SetPosX(440.0f);
	}
	if (m_camera_sensi == SENS_LOW - m_f)
	{
		SetPosX(535.0f);
	}
	else if (m_camera_sensi == SENS_LOW - m_f * 2)
	{
		SetPosX(631.0f);
	}
	else if (m_camera_sensi == SENS_LOW - m_f * 3)
	{
		SetPosX(727.0f);
	}
	else if (m_camera_sensi == SENS_LOW - m_f * 4)
	{
		SetPosX(823.0f);
	}
	else if (m_camera_sensi == SENS_LOW - m_f * 5)
	{
		SetPosX(919.0f);
	}
	else if (m_camera_sensi == SENS_LOW - m_f * 6)
	{
		SetPosX(1015.0f);
	}
	else if (m_camera_sensi == SENS_LOW - m_f * 7)
	{
		SetPosX(1111.0f);
	}
	else if (m_camera_sensi == SENS_LOW - m_f * 8)
	{
		SetPosX(1207.0f);
	}
	else if (m_camera_sensi == SENS_HIGHT)
	{
		SetPosX(1303.0f);
	}
}

SensiSlider::~SensiSlider()
{
	delete pLoadSensiSlider;
	pLoadSensiSlider = nullptr;
}

void SensiSlider::Update(void)
{
	m_camera_sensi = GetCameraSensi();


	//----------------------------------------------------
	// マウスでの選択
	// Date:2024/02/02	Auther:野村悠人
	//----------------------------------------------------
	float mouseX = GetMousePosX();
	float mouseY = GetMousePosY();
	
	if (IsMouseLeftTriggered()) 
	{	
		if (mouseY > POSITION.y - SIZE_HALF.y &&
			mouseY < POSITION.y + SIZE_HALF.y)
		{
			if (mouseX > GetPosX() - SIZE_HALF.x &&
				mouseX < GetPosX() + SIZE_HALF.x)
			{
				m_catch = true;
			}
		}
	}
	if (IsMouseLeftRelease()) 
	{
		m_catch = false;
	}

	if (m_catch) 
	{
		if (mouseX > 440.0f - SIZE_HALF.x &&
			mouseX < 440.0f + SIZE_HALF.x) 
		{
			SetPosX(440.0f);
			SetCameraSensi(SENS_LOW);
		}
		else if (mouseX > 535.0f - SIZE_HALF.x &&
			mouseX < 535.0f + SIZE_HALF.x) 
		{
			SetPosX(535.0f);
			SetCameraSensi(SENS_LOW - m_f);
		}
		else if (mouseX > 631.0f - SIZE_HALF.x &&
				mouseX < 631.0f + SIZE_HALF.x) 
		{
			SetPosX(631.0f);
			SetCameraSensi(SENS_LOW - m_f * 2);
		}
		else if (mouseX > 727.0f - SIZE_HALF.x &&
			mouseX < 727.0f + SIZE_HALF.x) 
		{
			SetPosX(727.0f);
			SetCameraSensi(SENS_LOW - m_f * 3);
		}
		else if (mouseX > 823.0f - SIZE_HALF.x &&
			mouseX < 823.0f + SIZE_HALF.x) 
		{
			SetPosX(823.0f);
			SetCameraSensi(SENS_LOW - m_f * 4);
		}
		else if (mouseX > 919.0f - SIZE_HALF.x &&
			mouseX < 919.0f + SIZE_HALF.x) 
		{
			SetPosX(919.0f);
			SetCameraSensi(SENS_LOW - m_f * 5);
		}
		else if (mouseX > 1015.0f - SIZE_HALF.x &&
			mouseX < 1015.0f + SIZE_HALF.x) 
		{
			SetPosX(1015.0f);
			SetCameraSensi(SENS_LOW - m_f * 6);
		}
		else if (mouseX > 1111.0f - SIZE_HALF.x &&
			mouseX < 1111.0f + SIZE_HALF.x) 
		{
			SetPosX(1111.0f);
			SetCameraSensi(SENS_LOW - m_f * 7);
		}
		else if (mouseX > 1207.0f - SIZE_HALF.x &&
			mouseX < 1207.0f + SIZE_HALF.x)
		{
			SetPosX(1207.0f);
			SetCameraSensi(SENS_LOW - m_f * 8);
		}
		else if (mouseX > 1303.0f - SIZE_HALF.x &&
			mouseX < 1303.0f + SIZE_HALF.x)
		{
			SetPosX(1303.0f);
			SetCameraSensi(SENS_HIGHT);
		}
	}

	//440,535,631,727,823,919,1015,1111,1207,1303
	if (GetKeyboardTrigger(DIK_LEFT))
	{
		if (m_camera_sensi == SENS_LOW - m_f)
		{
			SetPosX(440.0f);
			SetCameraSensi(SENS_LOW);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 2)
		{
			SetPosX(535.0f);
			SetCameraSensi(SENS_LOW - m_f);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 3)
		{
			SetPosX(631.0f);
			SetCameraSensi(SENS_LOW - m_f * 2);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 4)
		{
			SetPosX(727.0f);
			SetCameraSensi(SENS_LOW - m_f * 3);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 5)
		{
			SetPosX(823.0f);
			SetCameraSensi(SENS_LOW - m_f * 4);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 6)
		{
			SetPosX(919.0f);
			SetCameraSensi(SENS_LOW - m_f * 5);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 7)
		{
			SetPosX(1015.0f);
			SetCameraSensi(SENS_LOW - m_f * 6);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 8)
		{
			SetPosX(1111.0f);
			SetCameraSensi(SENS_LOW - m_f * 7);
		}
		else if (m_camera_sensi == SENS_HIGHT)
		{
			SetPosX(1207.0f);
			SetCameraSensi(SENS_LOW - m_f * 8);
		}
	}

	if (GetKeyboardTrigger(DIK_RIGHT))
	{
		if (m_camera_sensi == SENS_LOW)
		{
			SetPosX(535.0f);
			SetCameraSensi(SENS_LOW - m_f);
		}
		else if (m_camera_sensi == SENS_LOW - m_f)
		{
			SetPosX(631.0f);
			SetCameraSensi(SENS_LOW - m_f * 2);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 2)
		{
			SetPosX(727.0f);
			SetCameraSensi(SENS_LOW - m_f * 3);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 3)
		{
			SetPosX(823.0f);
			SetCameraSensi(SENS_LOW - m_f * 4);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 4)
		{
			SetPosX(919.0f);
			SetCameraSensi(SENS_LOW - m_f * 5);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 5)
		{
			SetPosX(1015.0f);
			SetCameraSensi(SENS_LOW - m_f * 6);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 6)
		{
			SetPosX(1111.0f);
			SetCameraSensi(SENS_LOW - m_f * 7);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 7)
		{
			SetPosX(1207.0f);
			SetCameraSensi(SENS_LOW - m_f * 8);
		}
		else if (m_camera_sensi == SENS_LOW - m_f * 8)
		{
			SetPosX(1303.0f);
			SetCameraSensi(SENS_HIGHT);
		}
	}
}

void SensiSlider::Draw(void)
{
	DrawSpriteLeftTop(pLoadSensiSlider->GetTexture(), GetPos().x, GetPos().y, GetSize().x, GetSize().y,
		pLoadSensiSlider->GetU(),  pLoadSensiSlider->GetV(),
		pLoadSensiSlider->GetUW(), pLoadSensiSlider->GetVH(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}

SensiEscape::SensiEscape()
{
	pLoadEscape = new LoadEscape();
	pLoadEscape->LoadPlaneTexture();
	SetPos(POSITION);
	SetSize(SIZE);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

SensiEscape::~SensiEscape()
{
	delete pLoadEscape;
	pLoadEscape = nullptr;
}

void SensiEscape::Update(void)
{
}

void SensiEscape::Draw(void)
{
	DrawSpriteLeftTop(pLoadEscape->GetTexture(), GetPos().x, GetPos().y, GetSize().x, GetSize().y,
		pLoadEscape->GetU(), pLoadEscape->GetV(),
		pLoadEscape->GetUW(),pLoadEscape->GetVH(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}
