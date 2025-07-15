//===============================================
//ポーズ制御[pause.cpp]
//class：上位モジュール
//Date:2024/01/19	Auther:嶋田虎士郎
//===============================================
#include "memleakcheck.h" // メモリリークチェック用のヘッダーを最初にインクルード

#ifdef _DEBUG
#pragma push_macro("new")
#undef new
#endif


#include "planePolygon.h"
#include "camera.h"

#ifdef _DEBUG
#pragma pop_macro("new")
#endif
#include "pause.h"
#include "input.h"
#include "fade.h"

Pause::Pause()
{
	pUIPause = new UIPause();
	m_pause = PAUSE1;
	m_StageSelectTexNo = LoadTexture((char*)"data/TEXTURE/pause_ReturnStageSelect.png");
	m_SensiSelectTexNo = LoadTexture((char*)"data/TEXTURE/pause_SensiSet.png");
	m_GameSelectTexNo = LoadTexture((char*)"data/TEXTURE/pause_ReturnGame.png");
	PauseFlag = false;
}

Pause::~Pause()
{
	delete pUIPause;
	pUIPause = nullptr;
	delete pSensi;
	pSensi = nullptr;
}

void Pause::Update(void)
{
	pUIPause->Update();
	if (pSensi)
	{
		pSensi->Update();
	}
	//----------------------------------------------------
	// マウスでの選択
	// Date:2024/02/02	Auther:野村悠人
	//----------------------------------------------------
	else 
	{
		float mouseX = GetMousePosX();
		float mouseY = GetMousePosY();
		float dispWidth = GetWindowSize().x;

		if (mouseX > dispWidth / 2.0f - SIZE_HALF.x &&
			mouseX < dispWidth / 2.0f + SIZE_HALF.x)
		{
			// ステージ選択
			if (mouseY > STAGE_POS_Y - SIZE_HALF.y &&
				mouseY < STAGE_POS_Y + SIZE_HALF.y)
			{
				m_pause = PAUSE1;
			}
			// 感度設定
			if (mouseY > SENSI_POS_Y - SIZE_HALF.y &&
				mouseY < SENSI_POS_Y + SIZE_HALF.y)
			{
				m_pause = PAUSE2;

			}
			// ゲームへ戻る
			if (mouseY > GAME_POS_Y - SIZE_HALF.y &&
				mouseY < GAME_POS_Y + SIZE_HALF.y)
			{
				m_pause = PAUSE3;
			}
		}

		if (GetKeyboardTrigger(DIK_UP))
		{
			if (m_pause == PAUSE2)
			{
				m_pause = PAUSE1;
			}
			else if (m_pause == PAUSE3)
			{
				m_pause = PAUSE2;
			}
		}

		if (GetKeyboardTrigger(DIK_DOWN))
		{
			if (m_pause == PAUSE1)
			{
				m_pause = PAUSE2;
			}
			else if (m_pause == PAUSE2)
			{
				m_pause = PAUSE3;
			}
		}

		if (GetKeyboardTrigger(DIK_RETURN) || IsMouseLeftTriggered())
		{
			if (m_pause == PAUSE1)
			{
				SetFadeTime(1.0f);
				SetFadeScene(SCENE_STAGE_SELECT);
				if (GetCameraState() == STATE_TPS)
				{
					ChangeCameraState();
				}
			}
			else if (m_pause == PAUSE2)
			{
				pSensi = new Sensi();
			}
			else if(m_pause == PAUSE3)
			{
				PauseFlag = false;
			}
		}
	}
}

void Pause::Draw(void)
{
	pUIPause->Draw();
	if (m_pause == PAUSE1)
	{
		DrawSprite(m_StageSelectTexNo, POS_X, STAGE_POS_Y, 1000.0f, 250.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_SensiSelectTexNo, POS_X, SENSI_POS_Y, SIZE.x,  SIZE.y, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_GameSelectTexNo,  POS_X, GAME_POS_Y,  SIZE.x,  SIZE.y, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (m_pause == PAUSE2)
	{
		DrawSprite(m_StageSelectTexNo, POS_X, STAGE_POS_Y, SIZE.x,  SIZE.y, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_SensiSelectTexNo, POS_X, SENSI_POS_Y, 1000.0f, 250.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_GameSelectTexNo,  POS_X, GAME_POS_Y,  SIZE.x,  SIZE.y, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (m_pause == PAUSE3)
	{
		DrawSprite(m_StageSelectTexNo, POS_X, STAGE_POS_Y, SIZE.x,  SIZE.y, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_SensiSelectTexNo, POS_X, SENSI_POS_Y, SIZE.x,  SIZE.y, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_GameSelectTexNo,  POS_X, GAME_POS_Y,  1000.0f, 250.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	if (pSensi)
	{
		pSensi->Draw();
	}
	if (GetKeyboardTrigger(DIK_ESCAPE))
	{
		delete pSensi;
		pSensi = nullptr;
	}
}

//ポーズ画面
UIPause::UIPause()
{
	pLoadUIPause = new LoadUIPause();
	pLoadUIPause->LoadPlaneTexture();
	SetPos(POSITION);
	SetSize(SIZE);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

UIPause::~UIPause()
{
	delete pLoadUIPause;
	pLoadUIPause = nullptr;
}

void UIPause::Update(void)
{
}

void UIPause::Draw(void)
{
	DrawSpriteLeftTop(pLoadUIPause->GetTexture(), GetPos().x, GetPos().y, GetSize().x, GetSize().y,
		pLoadUIPause->GetU(), pLoadUIPause->GetV(),
		pLoadUIPause->GetUW(), pLoadUIPause->GetVH(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
}
