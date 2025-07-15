//===============================================
//プレイヤー制御[player.cpp]
//class：上位モジュール
//Date:2023/12/05	Auther:林祐也
//===============================================
#include "memleakcheck.h" // メモリリークチェック用のヘッダーを最初にインクルード

#ifdef _DEBUG
#pragma push_macro("new")
#undef new
#endif

#include "main.h" // DirectX のヘッダーが含まれている
#include "polygon.h"
#include "camera.h"
#include "collision.h"

#ifdef _DEBUG
#pragma pop_macro("new")
#endif
#include "player.h"
#include "input.h"
#include "sikigami.h"
#include "ui.h"

Player::Player()
{
	m_moveobjcomponent = new PlayerMove();
	m_movel = new LoadPINModel;
	m_movel->Loadmodel();

	//SetPos(D3DXVECTOR3(150.0f, 0.0f, 250.0f));
	SetSize(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	SetScl(D3DXVECTOR3(5.0f, 5.0f, 5.0f));

	//SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	centerX = screenWidth / 2;
	centerY = screenHeight / 2;

}

Player::~Player()
{
	m_movel->_model = nullptr;
	delete m_movel;
	m_movel = nullptr;
	delete m_moveobjcomponent;
	for (auto it = m_inventories.begin(); it != m_inventories.end(); ) {
		delete* it;  // オブジェクトを削除
		it = m_inventories.erase(it);  // リストから要素を削除し、次の要素のイテレータを取得
	}

}

void Player::Update(void)
{
	//SetOldPos(GetPos());
	if (m_changetFlag)
	{
		

		// 視点の切り替え
		if (GetKeyboardTrigger(DIK_TAB))
		{
			if (GetCameraState() == STATE_TPS) {
				m_pSelectObj->SetSclX(SIKIGAMI_SCL);
				m_pSelectObj->SetSclY(SIKIGAMI_SCL);
				m_pSelectObj->SetSclZ(SIKIGAMI_SCL);
				m_pSelectObj = m_pOnmyouji;
				possessionCnt = 0;
				ChangeCameraState();
				m_selectNum = -1;
				m_changetFlag = false;
				possessionCoolTimeCnt = 0;
				SetSaihyouiFlag(false);
				for (auto& sikigami : m_sikigamiobj) {
					sikigami->SetPossessionFlag(false);
					sikigami->m_underatk = false;
					sikigami->SetPos(GetPos() + D3DXVECTOR3{0.0f, 20.0f, 0.0f});
				}
			}
			//再憑依可能flagがtrueになったら
			if (GetSaihyouiFlag())
			{
				if (GetCameraState() == STATE_OVERLOOK) {
					ChangeCameraState();
					m_selectNum = -1;
					m_changetFlag = false;
				}
			}

		}
	}

	

	if (GetCameraState() == STATE_OVERLOOK)
	{
		StopSound(GetSoundID(SE_SIKI_MOVE));
		possessionCoolTimeCnt++;
		if (!GetSaihyouiFlag())
		{
			// クールタイムが○秒になったら対応したNumberを送る
			if (possessionCoolTimeCnt == 300) // 5秒たったら
			{
				m_HyouiCoolTimeNumber = 0;// 0を表示する
			}
			else if (possessionCoolTimeCnt == 240)
			{
				m_HyouiCoolTimeNumber = 1;
			}
			else if (possessionCoolTimeCnt == 180)
			{
				m_HyouiCoolTimeNumber = 2;
			}
			else if (possessionCoolTimeCnt == 120)
			{
				m_HyouiCoolTimeNumber = 3;
			}
			else if (possessionCoolTimeCnt == 60)
			{
				m_HyouiCoolTimeNumber = 4;
			}
			else if (possessionCoolTimeCnt == 1)
			{
				m_HyouiCoolTimeNumber = 5;
			}
		}

		// 再憑依可能になったらfragをtrueに
		if (possessionCoolTimeCnt >= POSSESSION_COOLTIME)
		{
			SetSaihyouiFlag(true);
		}
		// 式神の選択
		SikigamiSelect();

		// カメラをプレイヤーの真上に設定
		SetCameraOverLook(m_pOnmyouji->GetPos());

		// 移動処理
		OverLookMove();

		// プレイヤーの座標/角度を陰陽師の座標/角度と同じに
		SetPos(m_pOnmyouji->GetPos());
		SetRot(m_pOnmyouji->GetRot());

		for (auto& sikigami : m_sikigamiobj) {
			sikigami->SetRotY(atan2f(clickPos.y, clickPos.x));
			sikigami->SetPos(sikigami->GetPos() + m_moveobjcomponent->GetVel());
			sikigami->SetPosY(GetPosY());
		}
	}
	else if (GetCameraState() == STATE_TPS)
	{
		StopSound(GetSoundID(SE_ONMYO_MOVE));
		//憑依時間のカウント
		possessionCnt++;
		// 憑依の解除のときに番号選択をしなくてもいいように
		m_changetFlag = true;
		//カメラの角度をプレイヤーに対応させる
		m_pSelectObj->SetRotY(GetCamera()->rot.y);
		
		//gm_pSelectObj->GetRot()

		//TPSカメラの設定/原神のカメラに近い
		SetCameraFreeLookTPS(m_pSelectObj->GetPos());
		
		//移動処理/内容はmoveobjに/移動速度の設定はmoveobjのplayermoveに
		m_pSelectObj->SetPosZ(m_pSelectObj->GetPos().z + m_moveobjcomponent->Move().z);
		m_pSelectObj->SetPosX(m_pSelectObj->GetPos().x + m_moveobjcomponent->Move().x);

		if (possessionCnt >=2)
		{
			m_pSelectObj->SetSclX(SIKIGAMI_SCL * 1.55f);
			m_pSelectObj->SetSclY(SIKIGAMI_SCL * 1.55f);
			m_pSelectObj->SetSclZ(SIKIGAMI_SCL * 1.55f);
		}
		if (m_moveobjcomponent->Move().z != 0 && m_moveobjcomponent->Move().x != 0)PlaySound(GetSoundID(SE_SIKI_MOVE), -1);
		else {
			StopSound(GetSoundID(SE_SIKI_MOVE));
		}
		SetPos(m_pSelectObj->GetPos());
		//憑依の制限時間
		if (possessionCnt >= POSSESSION_MAX_TIME)
		{
			m_pSelectObj->SetSclX(SIKIGAMI_SCL);
			m_pSelectObj->SetSclY(SIKIGAMI_SCL);
			m_pSelectObj->SetSclZ(SIKIGAMI_SCL);
			m_pSelectObj = m_pOnmyouji;
			possessionCnt = 0;
			ChangeCameraState();
			SetSaihyouiFlag(false);
			m_selectNum = -1;
			m_changetFlag = false;
			possessionCoolTimeCnt = 0;
			for (auto& sikigami : m_sikigamiobj) {
				sikigami->SetPossessionFlag(false);
			}
		}
		//式神の体力が０になったら戻す
		for (auto sikigami : m_sikigamiobj) {
			//憑依中のやつだけ
			if (sikigami->GetPossessionFlag())
			{
				//式神が幽霊状態になったら
				if (!sikigami->GetSikigamiFlag()) {
					m_pSelectObj->SetSclX(SIKIGAMI_SCL);
					m_pSelectObj->SetSclY(SIKIGAMI_SCL);
					m_pSelectObj->SetSclZ(SIKIGAMI_SCL);
					m_pSelectObj = m_pOnmyouji;
					possessionCnt = 0;
					ChangeCameraState();
					m_selectNum = -1;
					m_changetFlag = false;
					possessionCoolTimeCnt = 0;
					for (auto& sikigami : m_sikigamiobj) {
						sikigami->SetPossessionFlag(false);
					}
				}
			}
		}
	}

/*===================================================================================
	アイテム処理
	Date：2023/12/31
	制作者：譚偉進
====================================================================================*/
	//アイテム拾う処理
	SikigamiKobushi* pSin = dynamic_cast<SikigamiKobushi*>(m_pSelectObj);			//現在使用キャラ
	SikigamiTachi* pTachi = dynamic_cast<SikigamiTachi*>(m_pSelectObj);
	SikigamiYoujyutsu* pYoujyutsu = dynamic_cast<SikigamiYoujyutsu*>(m_pSelectObj);
	Onmyouji* pOnmyouji = dynamic_cast<Onmyouji*>(m_pSelectObj);
	for(auto item:m_items){
		if(item->GetFlag()){

			if(pSin){
				CollisionPoly(pSin, pSin->GetModelObj());
				CollisionPoly(item, item->GetModel());
				if (CollisionBB(pSin->GetModelTrasformMinPos(), pSin->GetModelTrasformMaxPos(), item->GetModelTrasformMinPos(), item->GetModelTrasformMaxPos())) {
					m_inventories.push_back(new Inventory(item->GetItemType()));
					item->SetFlag(false);
					continue;
				}
			}
			else if (pTachi) {
				CollisionPoly(pTachi, pTachi->GetModelObj());
				CollisionPoly(item, item->GetModel());
				if (CollisionBB(pTachi->GetModelTrasformMinPos(), pTachi->GetModelTrasformMaxPos(), item->GetModelTrasformMinPos(), item->GetModelTrasformMaxPos())) {
					m_inventories.push_back(new Inventory(item->GetItemType()));
					item->SetFlag(false);
					continue;
				}
			}
			else if (pOnmyouji) {
				CollisionPoly(pOnmyouji, pOnmyouji->GetModelObj());
				CollisionPoly(item, item->GetModel());
				if (CollisionBB(pOnmyouji->GetModelTrasformMinPos(), pOnmyouji->GetModelTrasformMaxPos(), item->GetModelTrasformMinPos(), item->GetModelTrasformMaxPos())) {
					m_inventories.push_back(new Inventory(item->GetItemType()));
					item->SetFlag(false);
					continue;
				}
			}
			else if (pYoujyutsu) {
				CollisionPoly(pYoujyutsu, pYoujyutsu->GetModelObj());
				CollisionPoly(item, item->GetModel());
				if (CollisionBB(pYoujyutsu->GetModelTrasformMinPos(), pYoujyutsu->GetModelTrasformMaxPos(), item->GetModelTrasformMinPos(), item->GetModelTrasformMaxPos())) {
					m_inventories.push_back(new Inventory(item->GetItemType()));
					item->SetFlag(false);
					continue;
				}
			}
		}
	}
	//アイテム使う処理
	if (GetKeyboardTrigger(DIK_F)) {
		if (m_inventories.size() != 0) {
			for (auto inventory : m_inventories) {
				if (inventory != nullptr && inventory->GetFlag()) {
					if (pSin) {
						pSin->HPRecovery();
						inventory->UseItem();
					}
					else if (pTachi) {
						pTachi->HPRecovery();
						inventory->UseItem();
					}
					else if (pOnmyouji) {
						pOnmyouji->HPRecovery();
						inventory->UseItem();
					}
					else if (pYoujyutsu) {
						pYoujyutsu->HPRecovery();
						inventory->UseItem();
					}
					
				}
			}
		}
	}
}

void Player::Draw(void)
{

	if (m_changetFlag)
	{
		for (auto sikigami : m_sikigamiobj) {
			if (sikigami->IsSameNumber(m_selectNum))
			{
				DrawPolygon(*m_movel->_model, sikigami->GetPos(), GetSize(), GetRot(), GetScl(), GetMtxWorld());
				break;
			}
			
		}

	}
	//bool cameraPossessionFlag = GetCameraPossession();
	//if (!cameraPossessionFlag)
	//{
	//	// ポリゴンの描画処理
	//	DrawPlayerPolygon(m_modelcomponent->m_model, GetPos(), GetSize(), GetRot(), GetScl(), GetMtxWorld());
	//}
	//else if(cameraPossessionFlag)
	//{
	//	DrawPolygon(m_modelcomponent->m_model, GetPos(), GetSize(), GetRot(), GetScl(), GetMtxWorld());
	//}

	//インベントリは毎回解放する
	for (auto it = m_inventories.begin(); it != m_inventories.end(); ) {
		if (!(*it)->GetFlag()) {
			delete* it;  // メモリ解放
			it = m_inventories.erase(it);  // 削除された要素の次の要素を指すイテレータを取得
		}
		else {
			++it;
		}
	}

}

//---------------------------------------------------
//プレイヤーの回転にカメラが対応する
//Date:2023/11/27	Auther:林祐也
//---------------------------------------------------
void Player::PlayerTPS()
{//player.hにoldMousePosとmousePosを追加/SetCameraTPSをcamera.cppに追加

	// マウスカーソルを中央に移動させる
	SetCursorPos(centerX, centerY);

	SetCameraTPS(GetPos(), GetRot());
	//マウスの過去座標を格納
	oldMousePos = mousePos;
	//マウスの座標を格納
	mousePos = D3DXVECTOR2(GetMousePosX(), GetMousePosY());

	//中央から右にいる場合
	if (oldMousePos.x > centerX)
	{
		//元に戻る時は反応しないように
		if (mousePos.x > oldMousePos.x)
		{
			SetRotY(GetRot().y - (oldMousePos.x - mousePos.x) / SENS_MIDDLE);
		}
	}
	//中央から左にいる場合
	if (oldMousePos.x < centerX)
	{
		//元に戻る時は反応しないように
		if (mousePos.x > oldMousePos.x)
		{
			SetRotY(GetRot().y + (oldMousePos.x - mousePos.x) / SENS_MIDDLE);
		}
	}
	//中央から下にいる場合
	if (oldMousePos.y > centerY)
	{
		//元に戻る時は反応しないように
		if (mousePos.y < oldMousePos.y)
		{
			SetRotX(GetRot().x - (oldMousePos.y - mousePos.y) / SENS_MIDDLE);
		}
	}
	//中央から上にいる場合
	if (oldMousePos.y < centerY)
	{
		//元に戻る時は反応しないように
		if (mousePos.y < oldMousePos.y)
		{
			SetRotX(GetRot().x + (oldMousePos.y - mousePos.y) / SENS_MIDDLE);
		}
	}
	//回転Zは処理で使ってないので本来は設定しなくていいがバグの防止のために配置
	//回転Zの上限値設定
	if (GetRot().z >= 3.14f) {
		SetRotZ(3.14f);
	}
	//回転Zの下限値設定
	if (GetRot().z <= -3.14f) {
		SetRotZ(-3.14f);
	}
	//回転Xの上限値設定
	if (GetRot().x >= 3.14f / 2) {
		SetRotX(3.14f / 2);
	}
	//回転Xの下限値設定
	if (GetRot().x <= -3.14f / 2) {
		SetRotX(-3.14f / 2);
	}
}

void Player::PlayerFreeLook()
{
	//プレイヤーの方にカメラの座標を管理する用のやつを設定
	CAMERA cam;

	// マウスカーソルを中央に移動させる
	SetCursorPos(centerX, centerY);
	//マウスの過去座標を格納
	oldMousePos = mousePos;
	//マウスの座標を格納
	mousePos = D3DXVECTOR2(GetMousePosX(), GetMousePosY());
	//中央から右にいる場合
	if (oldMousePos.x > centerX)
	{
		//元に戻る時は反応しないように
		if (mousePos.x > oldMousePos.x)
		{
			cam.rot.y -= (oldMousePos.x - mousePos.x) / SENS_MIDDLE;
		}
	}
	//中央から左にいる場合
	if (oldMousePos.x < centerX)
	{
		//元に戻る時は反応しないように
		if (mousePos.x > oldMousePos.x)
		{
			cam.rot.y += (oldMousePos.x - mousePos.x) / SENS_MIDDLE;
		}
	}
	//中央から下にいる場合
	if (oldMousePos.y > centerY)
	{
		//元に戻る時は反応しないように
		if (mousePos.y < oldMousePos.y)
		{
			cam.rot.x -= (oldMousePos.y - mousePos.y) / SENS_MIDDLE;
		}
	}
	//中央から上にいる場合
	if (oldMousePos.y < centerY)
	{
		//元に戻る時は反応しないように
		if (mousePos.y < oldMousePos.y)
		{
			cam.rot.x += (oldMousePos.y - mousePos.y) / SENS_MIDDLE;
		}
	}
	//回転Zは処理で使ってないので本来は設定しなくていいがバグの防止のために配置
	//回転Zの上限値設定
	if (cam.rot.z >= 3.14f / 2) {
		cam.rot.z = (3.14f);
	}
	//回転Zの下限値設定
	if (cam.rot.z <= -3.14f / 2) {
		cam.rot.z = (-3.14f);
	}
	//回転Xの上限値設定
	if (cam.rot.x >= 3.14f / 2) {
		cam.rot.x = (3.14f / 2);
	}
	//回転Xの下限値設定
	if (cam.rot.x <= -3.14f / 2) {
		cam.rot.x = (-3.14f / 2);
	}

	SetCameraTPS(cam.pos, cam.rot);
}

//---------------------------------------------------
//プレイヤー神視点移動処理
//Date:2023/11/25	Auther:野村悠人
//---------------------------------------------------
void Player::OverLookMove()
{
	// ウィンドウのサイズの取得
	D3DXVECTOR2 windowSize = GetWindowSize();

	// 左クリック押した時
	if (IsMouseLeftTriggered())
	{
		PlaySound(GetSoundID(SE_PIN), 0);
		// 移動フラグをONに
		m_moveFlag = true;

		// クリックした時のマウス座標を格納
		clickPos.x = (GetMousePosX() - windowSize.x / 2.0f) * WINDOW_SCALE;
		clickPos.y = (GetMousePosY() - windowSize.y / 2.0f) * WINDOW_SCALE;

		// クリックした座標をワールド内の座標と同じに
		// XにZ,YにXを入れているのは、俯瞰時はカメラが90°右に回転するため
		mousePos.x = clickPos.x + m_pOnmyouji->GetPos().z;
		mousePos.y = clickPos.y + m_pOnmyouji->GetPos().x;

		// プレイヤーの向きをクリックした座標に向ける
		m_pOnmyouji->SetRotY(atan2f(clickPos.y, clickPos.x));

		// ピンUIのフラグをオンに
		m_pUiPin->SetFlag(true);
		// ピンUIの初期座標のセット
		m_pUiPin->SetPos(D3DXVECTOR3(GetMousePosX(),GetMousePosY(), 0.0f));
	}

	// 移動フラグがONの時
	if (m_moveFlag)
	{
		// 現在のプレイヤー座標とクリックした座標間の横と縦の辺の長さを取得
		// ここもXとZが逆なのはカメラが回転してるため
		D3DXVECTOR2 direction = mousePos - D3DXVECTOR2(m_pOnmyouji->GetPos().z, m_pOnmyouji->GetPos().x);
		// 三平方の定理で現在のプレイヤー座標とクリックした座標間の斜辺の長さを取得
		float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

		// 斜辺の長さが一定値より大きい時に移動量の計算
		if (STOP_DISTANCE < distance) 
		{
			PlaySound(GetSoundID(SE_ONMYO_MOVE), -1);
			// 移動量の計算
			m_pOnmyouji->GetMoveObj()->SetVelX(PLAYER_MOVE_SPEED * sinf(m_pOnmyouji->GetRot().y));
			m_pOnmyouji->GetMoveObj()->SetVelZ(PLAYER_MOVE_SPEED * cosf(m_pOnmyouji->GetRot().y));
		
		}
		else
		{
			m_pOnmyouji->GetMoveObj()->SetVelX(0.0f);
			m_pOnmyouji->GetMoveObj()->SetVelZ(0.0f);
			// 移動フラグをOFFに
			m_moveFlag = false;
			StopSound(GetSoundID(SE_ONMYO_MOVE));
			m_pUiPin->SetFlag(false);
		
		}

		for (auto& sikigami : m_sikigamiobj) {
			sikigami->SetRotY(atan2f(clickPos.y, clickPos.x));
			if (!sikigami->m_underatk) {
				sikigami->SetPos(sikigami->GetPos() + m_pOnmyouji->GetMoveObj()->GetVel());
			}
		}

	}

	m_pOnmyouji->SetPos(m_pOnmyouji->GetPos() + m_pOnmyouji->GetMoveObj()->GetVel());

	// ピンUIの座標をワールド座標に変更
	if (m_pUiPin->GetFlag()) {
		m_pUiPin->SetPos(D3DXVECTOR3(m_pUiPin->GetPos().x - m_pOnmyouji->GetMoveObj()->GetVel().z/ WINDOW_SCALE,
			m_pUiPin->GetPos().y- m_pOnmyouji->GetMoveObj()->GetVel().x/ WINDOW_SCALE,
			0.0f));
	}
}

//---------------------------------------------------
//式神選択
//Date:2023/12/05	Auther:野村悠人
//---------------------------------------------------
void Player::SikigamiSelect(void)
{
	// 式神を選択している状態でもう一度同じ式神を選択したとき、選択なしの状態にする
	switch (m_selectNum) {
	case 1:
		if (GetKeyboardTrigger(DIK_1))
		{
			m_changetFlag = false;
			m_selectNum = -1;
			return;
		}
		break;
	case 2:
		if (GetKeyboardTrigger(DIK_2))
		{
			m_changetFlag = false;
			m_selectNum = -1;
			return;
		}
		break;
	case 3:
		if (GetKeyboardTrigger(DIK_3))
		{
			m_changetFlag = false;
			m_selectNum = -1;
			return;
		}
		break;
	case 4:
		if (GetKeyboardTrigger(DIK_4))
		{
			m_changetFlag = false;
			m_selectNum = -1;
			return;
		}
		break;
	default:
		break;
	}

	// 式神の選択
	if (GetKeyboardTrigger(DIK_1))
	{
		m_changetFlag = true;
		m_selectFlag = true;
		m_selectNum = 1;
		m_selectpossessionnumber = 1;
	}
	if (GetKeyboardTrigger(DIK_2))
	{
		m_changetFlag = true;
		m_selectFlag = true;
		m_selectNum = 2;
		m_selectpossessionnumber = 2;
	}
	if (GetKeyboardTrigger(DIK_3))
	{
		m_changetFlag = true;
		m_selectFlag = true;
		m_selectNum = 3;
		m_selectpossessionnumber = 3;
	}
	if (GetKeyboardTrigger(DIK_4))
	{
		m_changetFlag = true;
		m_selectFlag = true;
		m_selectNum = 4;
		m_selectpossessionnumber = 4;
	}

	// 式神を選択したかどうか
	if (m_selectFlag) 
	{
		m_selectFlag = false;

		// 選択した番号の式神を探す
		auto it = std::find_if(m_sikigamiobj.begin(), m_sikigamiobj.end(), [this](SikigamiObj* &p) {
			//付属している式神より大きい番号を選んだらはじく
			if (m_sikigamiobj.size() < m_selectNum)
			{
				m_changetFlag = false;//選択対象外の式神が対象の時は憑依できないように
				return false;
			}
			if (p->IsSameNumber(m_selectNum)) {
				if (!p->GetSikigamiFlag())//式神が幽霊状態の場合
				{
					m_changetFlag = false;//幽霊状態の式神が対象の時は憑依できないように
					return false;
				}
				p->SetPossessionFlag(true);//憑依された
					return true;
			}
			p->SetPossessionFlag(false);//憑依されていない
			return false;
		});

		// いなかった時何もせずに返す
		if (it == m_sikigamiobj.end()) return;

		// 選択した式神のポインターを格納
		m_pSelectObj = (*it);
	}
}

void Player::PlayerTakeDamage(float damage)
{
	if(GetCamera()->state == STATE_TPS){
	auto obj = dynamic_cast<SikigamiObj*>(m_pSelectObj);
	if (obj != nullptr) {

		obj->m_hp -= damage;
		//こぶし使いの時に必殺を使っていたら
		if (obj->GetSikigamiNumber() == 1&& obj->GetUltimateFlag()) {
			obj->m_hp += damage* ULTIMATE_REDUCTIONRATE;
		}
		return;
		}
	}
	else if(GetCamera()->state == STATE_OVERLOOK){
		m_pOnmyouji->MinusHP(damage);
	}
}
