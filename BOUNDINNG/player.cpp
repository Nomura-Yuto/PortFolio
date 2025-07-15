//=================================================================
// [player.cpp]
// �v���C���[����
// ����ҁF�쑺 �I�l	�X�V���F2023/09/04
//=================================================================
#include "main.h"
#include "controller.h"
#include "texture.h"
#include "sprite.h"
#include "player.h"

//=================================================================
// �}�N����`
//=================================================================
#define JUGE_ACCELERATION_Z (0.3f) // �����x����p
#define JUMP_HEIGHT			(150.0f)
#define JUMP_SPPED			(-25.0f)

#define JUGE_ANGLE	(5.0f)

#define PARTITION_PATTERN_X (5)	// �e�N�X�`���̕�����(��)
#define PARTITION_PATTERN_Y (5)	// �e�N�X�`���̕�����(�c)
#define PATTERN_WIDTH	(1.0f / PARTITION_PATTERN_X) // �\���e�N�X�`���̉���
#define PATTERN_HEIGHT	(1.0f / PARTITION_PATTERN_Y) // �\���e�N�X�`���̏c��

#define SOCORE_DISIT	(6)		// �X�R�A�̌���

//=================================================================
// �O���[�o���ϐ�
//=================================================================
unsigned int	g_PlayerTexture;	// �e�N�X�`��ID
PLAYER			g_Player;			// ����
Float3 Acceleration;
float JumpSpeed;

unsigned int g_ArrowTexture;
ARROW g_Arrow;

unsigned int	g_NumTexture;	// �e�N�X�`��ID
Float3 StartAngle;

//=================================================================
// �v���C���[����������
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
// �v���C���[�I������
//=================================================================
void UninitPlayer()
{

	UnloadTexture(g_PlayerTexture);

}


//=================================================================
// �v���C���[�A�b�v�f�[�g����
//=================================================================
void UpdatePlayer()
{

	g_Arrow.use = true;

	// ���Z�b�g�{�^��
	if (GetControllerTrigger(NpadButton::Y::Index))
	{
		StartAngle = GetControllerRightAngle();
	}

	// �����x�Z���T�[
	Acceleration = GetControllerRightAcceleration();

	if (Acceleration.z >= JUGE_ACCELERATION_Z && !g_Player.jump)
	{
		g_Player.jumpOldPos = g_Player.pos;
		g_Player.jump = true;
	}


	// �W���C���Z���T�[
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

	// ���ǔ���
	if (g_Player.pos.x < 460.0f + PLAYER_WIDTH / 2)
		g_Player.pos.x = 460.0f + PLAYER_WIDTH / 2;

	// �E�ǔ���
	if (g_Player.pos.x > 1460.0f - PLAYER_WIDTH / 2)
		g_Player.pos.x = 1460.0f - PLAYER_WIDTH / 2;

	// ��ǔ���
	if (g_Player.pos.y < 40.0f + PLAYER_HEIGHT / 2)
		g_Player.pos.y = 40.0f + PLAYER_HEIGHT / 2;

	// ���ǔ���
	if (g_Player.pos.y > (float)SCREEN_HEIGHT - PLAYER_HEIGHT / 2)
		g_Player.pos.y = (float)SCREEN_HEIGHT - PLAYER_HEIGHT / 2;

}

//=================================================================
// �v���C���[�`�揈��
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
// ����(�v���C���[)�̃A�h���X�擾�p�֐�(�Q�b�^�[)
//=================================================================
PLAYER* GetPlayer(void) 
{
	return &g_Player;
}


//=================================================================
// ���˂鏈��
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
// �����̌X���̐ݒ�
//=================================================================
void SetStartAngle(Float3 angle)
{
	StartAngle = angle;
}

