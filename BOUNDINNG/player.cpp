//=================================================================
// [player.cpp]
// プレイヤー制御
// 制作者：野村 悠人	更新日：2023/09/04
//=================================================================
#include "main.h"
#include "controller.h"
#include "texture.h"
#include "sprite.h"
#include "player.h"

//=================================================================
// マクロ定義
//=================================================================
#define JUGE_ACCELERATION_Z (0.3f) // 加速度判定用
#define JUMP_HEIGHT			(150.0f)
#define JUMP_SPPED			(-25.0f)

#define JUGE_ANGLE	(5.0f)

#define PARTITION_PATTERN_X (5)	// テクスチャの分割数(横)
#define PARTITION_PATTERN_Y (5)	// テクスチャの分割数(縦)
#define PATTERN_WIDTH	(1.0f / PARTITION_PATTERN_X) // 表示テクスチャの横幅
#define PATTERN_HEIGHT	(1.0f / PARTITION_PATTERN_Y) // 表示テクスチャの縦幅

#define SOCORE_DISIT	(6)		// スコアの桁数

//=================================================================
// グローバル変数
//=================================================================
unsigned int	g_PlayerTexture;	// テクスチャID
PLAYER			g_Player;			// 実体
Float3 Acceleration;
float JumpSpeed;

unsigned int g_ArrowTexture;
ARROW g_Arrow;

unsigned int	g_NumTexture;	// テクスチャID
Float3 StartAngle;

//=================================================================
// プレイヤー初期化処理
//=================================================================
void InitPlayer()
{

	g_PlayerTexture = LoadTexture("rom:/player.tga");
	g_NumTexture = LoadTexture("rom:/number.tga");
	
	g_Player.pos = MakeFloat2(960.0f, 990.0f);
	g_Player.size = MakeFloat2(PLAYER_WIDTH, PLAYER_HEIGHT);
	g_Player.vel = MakeFloat2(15.0f, 10.0f);
	g_Player.jumpOldPos = g_Player.pos;
	g_Player.displayPos = g_Player.pos;
	g_Player.angle = ANGLE_NORMAL;
	g_Player.jump = false;

	Acceleration = MakeFloat3(g_Player.pos.x, g_Player.pos.y, 0.0f);

	g_ArrowTexture = LoadTexture("rom:/mark_arrow_up.tga");
	g_Arrow.pos = MakeFloat2(g_Player.pos.x, g_Player.pos.y - 100.0f);
	g_Arrow.size = MakeFloat2(100.0f, 100.0f);
	g_Arrow.rot = 0.0f;
	g_Arrow.use = true;

	JumpSpeed = JUMP_SPPED;
}


//=================================================================
// プレイヤー終了処理
//=================================================================
void UninitPlayer()
{

	UnloadTexture(g_PlayerTexture);

}


//=================================================================
// プレイヤーアップデート処理
//=================================================================
void UpdatePlayer()
{

	g_Arrow.use = true;

	// リセットボタン
	if (GetControllerTrigger(NpadButton::Y::Index))
	{
		StartAngle = GetControllerRightAngle();
	}

	// 加速度センサー
	Acceleration = GetControllerRightAcceleration();

	if (Acceleration.z >= JUGE_ACCELERATION_Z && !g_Player.jump)
	{
		g_Player.jumpOldPos = g_Player.pos;
		g_Player.jump = true;
	}


	// ジャイロセンサー
	Float3 nowAngle = GetControllerRightAngle();
	Float3 angle = MakeFloat3(
		nowAngle.x - StartAngle.x,
		nowAngle.y - StartAngle.y,
		nowAngle.z - StartAngle.z
	);

	float Deg2Rad = 3.141592f / 180.0f;
	if (angle.y < -JUGE_ANGLE * Deg2Rad)
	{
		g_Player.angle = ANGLE_LEFT;
		g_Arrow.rot = -45.0f;
	}
	else if (angle.y > JUGE_ANGLE * Deg2Rad)
	{
		g_Player.angle = ANGLE_RIGHT;
		g_Arrow.rot = 45.0f;
	}
	else
	{
		g_Player.angle = ANGLE_NORMAL;
		g_Arrow.rot = 0.0f;
	}

	
	if (g_Player.jump)
	{
		JumpPlayer();
		g_Arrow.use = false;;
	}

	// 左壁判定
	if (g_Player.pos.x < 460.0f + PLAYER_WIDTH / 2)
		g_Player.pos.x = 460.0f + PLAYER_WIDTH / 2;

	// 右壁判定
	if (g_Player.pos.x > 1460.0f - PLAYER_WIDTH / 2)
		g_Player.pos.x = 1460.0f - PLAYER_WIDTH / 2;

	// 上壁判定
	if (g_Player.pos.y < 40.0f + PLAYER_HEIGHT / 2)
		g_Player.pos.y = 40.0f + PLAYER_HEIGHT / 2;

	// 下壁判定
	if (g_Player.pos.y > (float)SCREEN_HEIGHT - PLAYER_HEIGHT / 2)
		g_Player.pos.y = (float)SCREEN_HEIGHT - PLAYER_HEIGHT / 2;

}

//=================================================================
// プレイヤー描画処理
//=================================================================
void DrawPlayer()
{

	DrawSprite(g_PlayerTexture, g_Player.displayPos.x, g_Player.displayPos.y, 
				g_Player.size.x, g_Player.size.y,
				0.0f, 0.0f, 1.0f, 1.0f);

	if (g_Arrow.use) 
	{
		DrawSpriteColorRotate(g_ArrowTexture, g_Arrow.pos.x, g_Arrow.pos.y,
			g_Arrow.size.x, g_Arrow.size.y, 0.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f, g_Arrow.rot);
	}

}

//=================================================================
// 実体(プレイヤー)のアドレス取得用関数(ゲッター)
//=================================================================
PLAYER* GetPlayer(void) 
{
	return &g_Player;
}


//=================================================================
// 跳ねる処理
//=================================================================
void JumpPlayer(void)
{
	g_Player.displayPos.y += JumpSpeed;
	if (g_Player.displayPos.y <= g_Player.pos.y - JUMP_HEIGHT)
	{
		JumpSpeed *= -1.0f;
	}
	else if (g_Player.displayPos.y >= g_Player.pos.y)
	{
		JumpSpeed *= -1.0f;
		g_Player.displayPos = g_Player.pos;
		g_Player.jump = false;
	}
}

//=================================================================
// 初期の傾きの設定
//=================================================================
void SetStartAngle(Float3 angle)
{
	StartAngle = angle;
}

