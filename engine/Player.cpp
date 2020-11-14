#include "Player.h"
#include "SkinnedMesh.h"
#include "GraphicsEngine.h"
#include "Terrain.h"
#include "AppWindow.h"
#include "Bullet.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "ActorManager.h"
#include "AnmEnumeration.h"
#include "CameraManager.h"


Player::Player(bool has_shadow) : Actor(has_shadow)
{
	//メッシュとアニメーションの読み込み
	m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\CharacterRough\\lp.fbx", true, nullptr, D3D11_CULL_BACK);
	m_model->setAnimationCategory(Animation::Type::Player);
	m_model->loadAnimation(nullptr, Animation::Player::Idle,  L"..\\Assets\\CharacterRough\\lp_idle.fbx", true, true, 0, false, false);
	m_model->loadAnimation(nullptr, Animation::Player::Idle2, L"..\\Assets\\CharacterRough\\lp_idle2.fbx", true, true, 0, false, false);
	m_model->loadAnimation(nullptr, Animation::Player::Idle3, L"..\\Assets\\CharacterRough\\lp_idle3.fbx", true, true, 0, false, false);

	m_model->loadAnimation(nullptr, Animation::Player::Walk,		L"..\\Assets\\CharacterRough\\lp_walk.fbx");
	m_model->loadAnimation(nullptr, Animation::Player::WalkBackward,L"..\\Assets\\CharacterRough\\lp_walkback.fbx");
	m_model->loadAnimation(nullptr, Animation::Player::Run,			L"..\\Assets\\CharacterRough\\lp_run.fbx");
	m_model->loadAnimation(nullptr, Animation::Player::Stop,		L"..\\Assets\\CharacterRough\\lp_stop.fbx");
	m_model->loadAnimation(nullptr, Animation::Player::Jump,		L"..\\Assets\\CharacterRough\\lp_jump.fbx", false, true, 0, true, 0.99f);
	m_model->loadAnimation(nullptr, Animation::Player::LandToIdle,	L"..\\Assets\\CharacterRough\\lp_land.fbx", false, false);
	m_model->loadAnimation(nullptr, Animation::Player::LandHard,	L"..\\Assets\\CharacterRough\\lp_land_hard.fbx", false, false);
	m_model->loadAnimation(nullptr, Animation::Player::LandToRun,	L"..\\Assets\\CharacterRough\\lp_land_to_run.fbx", false, false);
	m_model->loadAnimation(nullptr, Animation::Player::Roll,		L"..\\Assets\\CharacterRough\\lp_roll.fbx", false, false);
	m_model->loadAnimation(nullptr, Animation::Player::DodgeBack,   L"..\\Assets\\CharacterRough\\lp_dodge_back.fbx", false, true, 0.8f);
	m_model->loadAnimation(nullptr, Animation::Player::Attack1,		L"..\\Assets\\CharacterRough\\lp_slash.fbx", false, true, 0.5f);
	m_model->loadAnimation(nullptr, Animation::Player::Attack2,		L"..\\Assets\\CharacterRough\\lp_kick.fbx", false, false);

	//アニメーションの初期設定
	m_model->setAnimation(-1);
}

Player::~Player()
{
}

void Player::update(float delta)
{
	//save the input for this frame
	m_input.w = AppWindow::getKeyState('W');
	m_input.s = AppWindow::getKeyState('S');
	m_input.a = AppWindow::getKeyState('A');
	m_input.d = AppWindow::getKeyState('D');
	m_input.m1 = AppWindow::getMouseTrigger(false);
	m_input.m2 = AppWindow::getMouseTrigger(true);
	m_input.space = AppWindow::getKeyState(VK_SPACE);
	m_input.lshift = AppWindow::getKeyState(VK_LSHIFT);
	m_input.lctrl = AppWindow::getKeyState(VK_LCONTROL);

	Vector3D cam_dir = CameraManager::get()->getCamera().getZDirection();
	//get rid of the y value since we want the 2D forward vector for comparisons with the player's 2d forward vector
	cam_dir.m_y = 0;
	cam_dir.normalize();
	//convert radian result to degrees like the actor class stores
	m_input.cam_angle = atan2(cam_dir.m_x, cam_dir.m_z) / 0.01745f;
	if (m_angle > 360) m_angle -= 360;
	if (abs(m_input.cam_angle + 360 - m_angle) < abs(m_input.cam_angle - m_angle)) m_input.cam_angle += 360;


	//更新関数の選択
	switch (m_state)
	{
	case PlayerState::Idle:
		idle(delta);
		break;
	case PlayerState::MoveForward:
		moveForward(delta);
		break;
	case PlayerState::MoveBackward:
		moveBackward(delta);
		break;
	case PlayerState::Stop:
		stop(delta);
		break;
	case PlayerState::Jump:
		jump(delta);
		break;
	case PlayerState::Land:
		//landing type is a dummy value since it will only be used once when switched to from the jump function
		land(delta, -1);
		break;
	case PlayerState::Roll:
		roll(delta);
		break;
	case PlayerState::DodgeBack:
		dodgeBack(delta);
		break;
	case PlayerState::Attack1:
		attack1(delta);
		break;
	case PlayerState::Attack2:
		attack2(delta);
		break;
	}
}

void Player::idle(float delta)
{
	if (m_state != m_previous_state)
	{
		m_previous_state = m_state;
		int anm = m_model->getAnimation();
		float anm_percent = m_model->getActiveAnmPercent();
		m_model->setAnimation(Animation::Player::Idle);
		m_model->setBlendAnimation(anm);
		m_model->setBlendAnmFrame(anm_percent);
		m_blend = 1.0f;
	}

	////attack transition
	//if (m_input.m1)
	//{
	//	m_state = PlayerState::Attack1;
	//	attack1(delta);
	//	return;
	//}


	//jump state transition
	if (m_input.space)
	{
		m_state = PlayerState::Jump;
		jump(delta);
		return;
	}

	//walk state transition
	if (m_input.w)
	{
		m_state = PlayerState::MoveForward;
		moveForward(delta);
		return;
	}
	//backwards walk transition
	if (m_input.s)
	{
		m_state = PlayerState::MoveBackward;
		moveBackward(delta);
		return;
	}



	if (m_input.a)
	{
		m_angle -= 360.0f * P_ROT_SPEED * delta;
	}
	if (m_input.d)
	{
		m_angle += 360.0f * P_ROT_SPEED * delta;
	}

	if (m_blend != 0)
	{
		m_blend = m_blend * (1.0f - (10 * delta)) + 0 * (10 * delta);
	}
	m_model->setBlend(m_blend);

	//choose a random idle animation to play each time one of 3 finishes
	if (m_model->getIfAnimFinished())
	{
		int choice = rand() % 3;
		int anm = 0;
		switch (choice)
		{
		case 0: 
			m_model->setAnimation(Animation::Player::Idle);
			break;
		case 1:
			m_model->setAnimation(Animation::Player::Idle2);
			break;
		case 2:
			m_model->setAnimation(Animation::Player::Idle3);
		}
	}
}

void Player::moveForward(float delta)
{
	if (m_state != m_previous_state)
	{
		m_model->setAnimation(Animation::Player::Walk);
		m_model->setBlendAnimation(Animation::Player::Run);
		m_previous_state = m_state;
	}

	////attack transition
	//if (m_input.m1)
	//{
	//	m_state = PlayerState::Attack1;
	//	m_momentum = 0;
	//	attack1(delta);
	//	return;
	//}

	//roll transition
	if (m_input.m2)
	{
		m_state = PlayerState::Roll;
		m_momentum = 0;
		roll(delta);
		return;
	}

	//jump state transition
	if (m_input.space)
	{
		m_state = PlayerState::Jump;
		jump(delta);
		return;
	}
	//idle transition
	if (!m_input.w)
	{
		m_state = PlayerState::Idle;
		m_momentum = 0;
	}




	float speed_limit = P_WSPEED;
	float accel = P_WACCEL;
	//run transition
	if (m_input.lshift)
	{
		speed_limit = P_RSPEED;
		accel = P_RACCEL;
	}

	if (m_momentum != speed_limit)
	{
		m_momentum = m_momentum * (1.0f - (accel * delta)) + speed_limit * (accel * delta);
	}

	//lerp to the same direction as the camera
	float target_angle = m_input.cam_angle;
	if (m_input.a)
	{
		target_angle -= 60.0f;
	}
	if (m_input.d)
	{
		target_angle += 60.0f;
	}

	m_angle = m_angle * (1.0f - delta * 6) + target_angle * delta * 6;


	Vector3D move = (getDirectionVector() * m_momentum) * delta;
	CameraManager::get()->moveCamera(move);
	m_pos += move;

	m_model->setBlendAnmFrame(m_model->getActiveAnmPercent());
	m_blend = max(0, (m_momentum - P_WSPEED) / (P_RSPEED - P_WSPEED));
	m_model->setBlend(m_blend);

	//for blending animations with different playback speeds, it is important to set a percent tick for a smooth transition
	float anm1_len = 0;
	float anm2_len = 0;
	m_model->getAnimationLengths(anm1_len, anm2_len);
	float tick_amount = (anm1_len * (1.0f - m_blend) + anm2_len * m_blend);
	//float tick_amount = 1000000;
	m_model->setAnmPercentTick((delta / tick_amount));
}

void Player::moveBackward(float delta)
{
	if (m_state != m_previous_state)
	{
		m_model->setAnimation(Animation::Player::WalkBackward);
		m_previous_state = m_state;
	}

	////attack transition
	//if (m_input.m1)
	//{
	//	m_state = PlayerState::Attack1;
	//	m_momentum = 0;
	//	attack1(delta);
	//	return;
	//}

	//dodge back transition
	if (m_input.m2)
	{
		m_state = PlayerState::DodgeBack;
		dodgeBack(delta);
		return;
	}

	//jump state transition
	if (m_input.space)
	{
		m_state = PlayerState::Jump;
		jump(delta);
		return;
	}

	if (!m_input.s)
	{
		m_state = PlayerState::Idle;
		m_momentum = 0;
	}

	if (m_momentum != -P_WSPEED * 0.8f)
	{
		m_momentum = m_momentum * (1.0f - (0.8f * P_WACCEL * delta)) + -P_WSPEED * (0.8f * (P_WACCEL * delta));
	}

	//lerp to the same direction as the camera
	float target_angle = m_input.cam_angle;
	if (m_input.a)
	{
		target_angle += 60.0f;
	}
	if (m_input.d)
	{
		target_angle -= 60.0f;
	}

	m_angle = m_angle * (1.0f - delta * 6) + target_angle * delta * 6;


	Vector3D move = (getDirectionVector() * m_momentum) * delta;
	CameraManager::get()->moveCamera(move);
	m_pos += move;
}

void Player::stop(float delta)
{
}

void Player::jump(float delta)
{
	if (m_state != m_previous_state)
	{
		m_model->setAnimation(Animation::Player::Jump);
		m_previous_state = m_state;
	}

	float speed_limit = P_WSPEED;
	float accel = P_WACCEL;
	//run transition
	if (m_input.lshift)
	{
		speed_limit = P_RSPEED;
		accel = P_RACCEL;
	}

	if (m_input.w)
	{
		if (m_momentum != speed_limit)
		{
			m_momentum = m_momentum * (1.0f - (accel * delta)) + speed_limit * (accel * delta);
		}
	}
	else if (m_input.s)
	{
		if (m_momentum != -speed_limit * 0.8f)
		{
			m_momentum = m_momentum * (1.0f - (0.8f * accel * delta)) + -P_WSPEED * (0.8f * (accel * delta));
		}
	}

	//lerp to the same direction as the camera
	m_angle = m_angle * (1.0f - delta * 6) + m_input.cam_angle * delta * 6;


	if (m_jump == false)
	{
		m_jump = true;
		m_jump_speed = P_JUMP_SPEED;
	}

	Vector3D move = (getDirectionVector() * m_momentum + Vector3D(0, m_jump_speed, 0)) * delta;
	CameraManager::get()->moveCamera(move);
	m_pos += move;

	if (m_jump)
	{
		m_jump_speed -= P_GRAVITY;

		if (m_pos.m_y < 0)
		{
			m_pos.m_y = 0;
			m_jump = false;
			m_jump_speed = 0;
			m_model->triggerAnimationFinish(true);
			
			//switch to land
			m_state = PlayerState::Land;
			int landing_type = Animation::Player::LandToIdle;
			if (m_jump_speed < -15.0f) Animation::Player::LandHard;
			m_momentum = 0;
			if (m_input.w)
			{
				m_momentum = P_WSPEED * 0.5f;
				landing_type = Animation::Player::LandToRun;
			}


			land(delta, landing_type);
			return;
		}
	}
}

void Player::land(float delta, int landing_type)
{
	if (m_state != m_previous_state)
	{
		m_model->setAnimation(landing_type);
		if (landing_type == Animation::Player::LandToRun) m_model->setBlendAnimation(Animation::Player::Run);
		else m_model->setBlendAnimation(Animation::Player::Idle);
		m_model->setBlendAnmFrame(0);
		m_previous_state = m_state;
	}

	int land_state = m_model->getAnimation();

	if (land_state == Animation::Player::LandToRun)
	{
		float speed_limit = P_WSPEED;
		float accel = P_WACCEL;

		if (m_input.lshift)
		{
			speed_limit = P_RSPEED;
			accel = P_RACCEL;
		}

		if (m_momentum != speed_limit)
		{
			m_momentum = m_momentum * (1.0f - (accel * delta)) + P_RSPEED * (accel * delta);
		}

		//lerp to the same direction as the camera
		m_angle = m_angle * (1.0f - delta * 6) + m_input.cam_angle * delta * 6;

		Vector3D move = (getDirectionVector() * m_momentum) * delta;
		CameraManager::get()->moveCamera(move);
		m_pos += move;
	}

	if (m_model->getIfAnimFinished())
	{
		if (land_state == Animation::Player::LandToRun)
		{
			m_state = PlayerState::MoveForward;
			moveForward(delta);
			return;
		}
		else
		{
			m_state = PlayerState::Idle;
			m_model->setAnimation(Animation::Player::Idle);
		}
	}

	m_blend = max((m_model->getActiveAnmPercent() - 0.85f) / 0.15f, 0.0f);
	m_model->setBlend(m_blend);
}

void Player::roll(float delta)
{
	if (m_state != m_previous_state)
	{
		m_model->setAnimation(Animation::Player::Roll);
		m_model->setBlendAnimation(Animation::Player::Idle);
		m_previous_state = m_state;
	}

	float percent_complete = m_model->getActiveAnmPercent();
	float ease = pow(1.0f - percent_complete, 2.0f);

	m_angle = m_input.cam_angle;
	Vector3D dir = getDirectionVector();
	Vector3D target_pos = dir * ease * P_ROLL_DIST + m_pos;

	Vector3D temp = m_pos;
	m_pos = Vector3D::lerp(m_pos, target_pos, ease);

	CameraManager::get()->moveCamera(m_pos - temp);



	if (m_model->getIfAnimFinished())
	{
		m_state = PlayerState::Idle;
		m_model->setAnimation(Animation::Player::Idle);
	}

	m_blend = max((m_model->getActiveAnmPercent() - 0.7f) / 0.3f, 0.0f);
	m_model->setBlend(m_blend);
}

void Player::dodgeBack(float delta)
{
	if (m_state != m_previous_state)
	{
		m_model->setAnimation(Animation::Player::DodgeBack);
		m_model->setBlendAnimation(Animation::Player::Idle);
		m_previous_state = m_state;
	}

	float percent_complete = m_model->getActiveAnmPercent();
	float ease = pow(1.0f - percent_complete, 2.0f);
	m_angle = m_input.cam_angle;
	Vector3D dir = getDirectionVector();
	Vector3D target_pos = dir * ease * P_ROLL_DIST * -0.5f + m_pos;

	//m_pos = Vector3D::lerp(m_pos, target_pos, ease);
	Vector3D temp = m_pos;
	m_pos = Vector3D::lerp(m_pos, target_pos, ease);

	CameraManager::get()->moveCamera(m_pos - temp);

	if (m_model->getIfAnimFinished() || (m_model->getIfAnimInterruptable() && (m_input.w || m_input.s )))
	{
		m_state = PlayerState::Idle;
		m_model->setAnimation(Animation::Player::Idle);
	}

	m_blend = max((m_model->getActiveAnmPercent() - 0.7f) / 0.3f, 0.0f);
	m_model->setBlend(m_blend);
}

void Player::attack1(float delta)
{
	if (m_state != m_previous_state)
	{
		m_model->setAnimation(Animation::Player::Attack1);
		m_model->setBlendAnimation(Animation::Player::Idle);
		m_previous_state = m_state;
	}

	if (m_model->getIfAnimInterruptable() && m_input.m1)
	{
		m_state = PlayerState::Attack2;
		attack2(delta);
	}

	if (m_model->getIfAnimFinished())
	{
		m_state = PlayerState::Idle;
		m_model->setAnimation(Animation::Player::Idle);
	}

	m_blend = max((m_model->getActiveAnmPercent() - 0.7f) / 0.3f, 0.0f);
	m_model->setBlend(m_blend);
}

void Player::attack2(float delta)
{
	if (m_state != m_previous_state)
	{
		m_model->setAnimation(Animation::Player::Attack2);
		m_model->setBlendAnimation(Animation::Player::Idle);
		m_previous_state = m_state;
	}

	if (m_model->getIfAnimFinished())
	{
		m_state = PlayerState::Idle;
		m_model->setAnimation(Animation::Player::Idle);
	}

	m_blend = max((m_model->getActiveAnmPercent() - 0.7f) / 0.3f, 0.0f);
	m_model->setBlend(m_blend);
}

void Player::render(float delta)
{
	m_model->renderMesh(delta, Vector3D(1, 1, 1), m_pos, Vector3D(0, m_angle * 0.01745f, 0), Shaders::LAMBERT_SPECULAR);
}

void Player::renderShadow(float delta)
{
	//アニメーションが複数回更新しないようにアニメーション生成速度を０に設定して描画する
	m_model->renderMesh(delta, Vector3D(1, 1, 1), m_pos, Vector3D(0, m_angle * 0.01745f, 0), Shaders::SHADOWMAP, false, 0);
}

void Player::imGuiWindow()
{
	ImGui::SetNextWindowSize(ImVec2(300, 200));
	ImGui::SetNextWindowPos(ImVec2(400, 0));

	//create the test window
	ImGui::Begin("Player Values");

	ImGui::DragFloat("Walk Acceleration", &P_WACCEL, 0.05f, 0.05f, 100.0f);
	ImGui::DragFloat("Run Acceleration", &P_RACCEL, 0.05f, 0.05f, 100.0f);
	ImGui::DragFloat("Max Walk Speed", &P_WSPEED, 0.05f, 0.05f, 100.0f);
	ImGui::DragFloat("Max Run Speed", &P_RSPEED, 0.05f, 0.05f, 100.0f);
	ImGui::DragFloat("Rotation Speed", &P_ROT_SPEED, 0.05f, 0.05f, 100.0f);
	ImGui::DragFloat("Jump Speed", &P_JUMP_SPEED, 0.05f, 0.05f, 100.0f);
	ImGui::DragFloat("Gravity", &P_GRAVITY, 0.05f, 0.05f, 100.0f);
	ImGui::DragFloat("Roll Distance", &P_ROLL_DIST, 0.05f, 0.00f, 10.0f);

	if (ImGui::Button("Walk", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Walk);
	if (ImGui::Button("Walkback", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::WalkBackward);
	if (ImGui::Button("Run", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Run);
	if (ImGui::Button("Jump", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Jump);
	if (ImGui::Button("Land", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::LandToIdle);
	if (ImGui::Button("Roll", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Roll);
	if (ImGui::Button("Attack", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Attack1);
	if (ImGui::Button("Attack2", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Attack2);
	if (ImGui::Button("Stop", ImVec2(200, 30))) m_model->setAnimation(-1);


	ImGui::End();
}

//void Player::animationTree()
//{
//	////if there is a new try animation value, attempt to update the model animation with it.
//	////if there is no try animation but there is a queue animation value, attempt to update the model with it.
//
//	//int anm_choice = -1;
//	//if (m_try_animation != -1) anm_choice = m_try_animation;
//	//else if (m_queued_animation != -1) anm_choice = m_queued_animation;
//
//	//bool animation_changed = false;
//	//bool can_interrupt = m_model->getIfAnimInterruptable();
//
//	////namespace Player
//	////{
//	////	enum
//	////	{
//	////		Idle,
//	////		Walk,
//	////		WalkBack,
//	////		Run,
//	////		Jump,
//	////		Land,
//	////		Roll,
//	////		Attack1,
//	////		Attack2,
//	////		MAX
//	////	};
//	////}
//
//	//if (anm_choice >= 0)
//	//{
//	//	switch (anm_choice)
//	//	{
//	//	case Animation::Player::Idle:
//	//		break;
//	//	case Animation::Player::Walk:
//	//		break;
//	//	case Animation::Player::WalkBack:
//	//		break;
//	//	case Animation::Player::Run:
//	//		break;
//	//	case Animation::Player::Jump:
//	//		break;
//	//	case Animation::Player::Land:
//	//		break;
//	//	case Animation::Player::Roll:
//	//		break;
//	//	case Animation::Player::Attack1:
//	//		break;
//	//	case Animation::Player::Attack2:
//	//		break;
//	//	}
//	//}
//}
