//===============================================
//ステージセレクト制御[stageselect.cpp]
//class：上位モジュール
//Date:2023/12/01	Auther:嶋田
//===============================================
#include "memleakcheck.h"
#ifdef _DEBUG
#pragma push_macro("new")
#undef new
#endif
#include "planePolygon.h"

#ifdef _DEBUG
#pragma pop_macro("new")
#endif
#include "stageselect.h"
#include "input.h"
#include "fade.h"
#include "gameobj.h"
#include "sound.h"

//コンストラクタ
Stage::Stage()
{
	StopSoundAll();
	
	pStageSelectBG = new StageSelectBG();
	pStageSelectArrow = new StageSelectArrow();
	m_MatsuTexNo = LoadTexture((char *)"data/TEXTURE/matsuText.png");
	m_TakeTexNo = LoadTexture((char *)"data/TEXTURE/takeText.png");
	m_UmeTexNo = LoadTexture((char*)"data/TEXTURE/UmeText.png");
	m_select = SELECT1;
	PlaySound(GetSoundID(BGM_TITLE), -1);
}
//デストラクタ
Stage::~Stage()
{
	delete pStageSelectArrow;
	pStageSelectArrow = nullptr;
	delete pStageSelectBG;
	pStageSelectBG = nullptr;
}
//更新処理
void Stage::Update(void)
{
	
	pStageSelectBG->Update();

	float mouseX = GetMousePosX();
	float mouseY = GetMousePosY();
	float dispWidth = GetWindowSize().x;

	//----------------------------------------------------
	// マウスでの選択
	// Date:2024/02/02	Auther:野村悠人
	//----------------------------------------------------
	if (mouseX > dispWidth / 2.0f - SIZE_HALF.x &&
		mouseX < dispWidth / 2.0f + SIZE_HALF.x)
	{
		// 松
		if (mouseY > MATSU_POS_Y - SIZE_HALF.y &&
			mouseY < MATSU_POS_Y + SIZE_HALF.y)
		{
 			m_select = SELECT1;
			pStageSelectArrow->SetPosY(MATSU_POS_Y);
		}
		// 竹
		if (mouseY > TAKE_POS_Y - SIZE_HALF.y &&
			mouseY < TAKE_POS_Y + SIZE_HALF.y)
		{
			m_select = SELECT2;
			pStageSelectArrow->SetPosY(TAKE_POS_Y);

		}
		// 梅
		if (mouseY >UME_POS_Y - SIZE_HALF.y &&
			mouseY <UME_POS_Y + SIZE_HALF.y)
		{
			m_select = SELECT3;
			pStageSelectArrow->SetPosY(UME_POS_Y);
		}
	}
	
	if (GetKeyboardTrigger(DIK_UP))
	{
		if (m_select == SELECT2)
		{
			m_select = SELECT1;
			pStageSelectArrow->SetPosY(MATSU_POS_Y);
		}
		else if (m_select == SELECT3)
		{
			m_select = SELECT2;
			pStageSelectArrow->SetPosY(TAKE_POS_Y);
		}
	}

	if (GetKeyboardTrigger(DIK_DOWN))
	{
		if (m_select == SELECT1)
		{
			m_select = SELECT2;
			pStageSelectArrow->SetPosY(TAKE_POS_Y);
		}
		else if (m_select == SELECT2)
		{
			m_select = SELECT3;
			pStageSelectArrow->SetPosY(UME_POS_Y);
		}
	}

	if (GetKeyboardTrigger(DIK_RETURN) || IsMouseLeftTriggered())
	{
		if (m_select == SELECT1)
		{
			SetStage(STAGE1);
			m_nextselect = SCENE_GAME;
			PlaySound(GetSoundID(SE_STAGE_CHANGE),0);
		}
		else if (m_select == SELECT2)
		{
			SetStage(STAGE2);
			m_nextselect = SCENE_GAME;
			PlaySound(GetSoundID(SE_STAGE_CHANGE), 0);
		}
		else if (m_select == SELECT3)
		{
			SetStage(STAGE3);
			m_nextselect = SCENE_GAME;
			PlaySound(GetSoundID(SE_STAGE_CHANGE), 0);
		}
	}
}
//描画処理
void Stage::Draw(void)
{
	pStageSelectBG->Draw();
	pStageSelectArrow->Draw();
	
	if (m_select == SELECT1)
	{
		DrawSprite(m_UmeTexNo, SCREEN_WIDTH / 2, 460.0f, 600.0f, 275.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_TakeTexNo, SCREEN_WIDTH / 2,  680.0f, 450.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_MatsuTexNo, SCREEN_WIDTH / 2,   900.0f, 450.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (m_select == SELECT2)
	{
		DrawSprite(m_UmeTexNo, SCREEN_WIDTH / 2, 460.0f, 450.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_TakeTexNo, SCREEN_WIDTH / 2,  680.0f, 600.0f, 275.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_MatsuTexNo, SCREEN_WIDTH / 2,   900.0f, 450.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (m_select == SELECT3)
	{
		DrawSprite(m_UmeTexNo, SCREEN_WIDTH / 2, 460.0f, 450.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_TakeTexNo, SCREEN_WIDTH / 2,  680.0f, 450.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		DrawSprite(m_MatsuTexNo, SCREEN_WIDTH / 2,   900.0f, 600.0f, 275.0f, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
}



StageSelectBG::StageSelectBG()
{
	pLoadStageSelectBG = new LoadStageSelectBG();
	pLoadStageSelectBG->LoadPlaneTexture();
	SetPos(POSITION);
	SetSize(SIZE);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

StageSelectBG::~StageSelectBG()
{
	delete pLoadStageSelectBG;
	pLoadStageSelectBG = nullptr;
}

void StageSelectBG::Update(void)
{
}

void StageSelectBG::Draw(void)
{
	DrawSpriteLeftTop(pLoadStageSelectBG->GetTexture(), GetPos().x, GetPos().y, GetSize().x, GetSize().y,
		pLoadStageSelectBG->GetU(), pLoadStageSelectBG->GetV(),
		pLoadStageSelectBG->GetUW(), pLoadStageSelectBG->GetVH(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}


StageSelectArrow::StageSelectArrow()
{
	pLoadStageSelectArrow = new LoadStageSelectArrow();
	pLoadStageSelectArrow->LoadPlaneTexture();
	SetPos(POSITION);
	SetSize(SIZE);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

StageSelectArrow::~StageSelectArrow()
{
	delete pLoadStageSelectArrow;
	pLoadStageSelectArrow = nullptr;
}

void StageSelectArrow::Update(void)
{
}

void StageSelectArrow::Draw(void)
{
	DrawSprite(pLoadStageSelectArrow->GetTexture(), GetPos().x, GetPos().y, GetSize().x, GetSize().y,
		pLoadStageSelectArrow->GetU(), pLoadStageSelectArrow->GetV(),
		pLoadStageSelectArrow->GetUW(), pLoadStageSelectArrow->GetVH(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}
