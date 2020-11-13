#pragma once
#include "Vector2D.h"
#include "high_resolution_timer.h"
#include "Actor.h"

namespace PlayerState
{
	enum
	{
		Idle,
		MoveForward,
		MoveBackward,
		Stop,
		Jump,
		Land,
		Roll,
		DodgeBack,
		Attack1,
		Attack2,
	};
}

struct PlayerInput
{
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;
		    
	bool m1 = false;
	bool m2 = false;

	bool space = false;
	bool lshift = false;
	bool lctrl = false;

	float cam_angle = 0;
};

class Player : public Actor
{
public:
	Player(bool has_shadow);
	~Player();
public:

	//=====================================================
	//   �X�V�֐�
	//-----------------------------------------------------
	void update(float delta) override;
	//-----------------------------------------------------
	void idle(float delta);
	void moveForward(float delta);
	void moveBackward(float delta);
	void stop(float delta);
	void jump(float delta);
	void land(float delta, int landing_type);
	void roll(float delta);
	void dodgeBack(float delta);
	void attack1(float delta);
	void attack2(float delta);
	//=====================================================

	void render(float delta) override;
	void renderShadow(float delta) override;

public:
	void imGuiWindow() override;
	//void animationTree() override;

private: 
	//�`��֘A�ϐ�
	SkinnedMeshPtr m_model;

	//�v���C���[�����֘A
	float P_WACCEL = 5.0f;
	float P_RACCEL = 10.0f;
	float P_WSPEED = 5.0f;
	float P_RSPEED = 15.0f;
	float P_ROT_SPEED = 0.2f;
	float P_JUMP_SPEED = 30.0f;
	float P_ROLL_DIST = 1.4f;
	float P_GRAVITY = 0.5f;

	//�Q�[�������֘A
	bool m_jump = false;
	float m_jump_speed = 0;
	float m_momentum = 0;
	float m_blend = 0;
	PlayerInput m_input;



private:
	friend class ActorManager;
};

