#include "CameraManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "AppWindow.h"
#include "ActorManager.h"

CameraManager* CameraManager::cm = nullptr;

CameraManager* CameraManager::get()
{
	if (cm == nullptr) cm = new CameraManager();
	return cm;
}

CameraManager::~CameraManager()
{

}

void CameraManager::update(const float& delta, const int& width, const int& height, bool exclude_Y_movement)
{
	updateInput();

	Matrix4x4 worldcam;
	worldcam.setIdentity();
	cb_world cc;
	cc.m_world.setIdentity();
	Matrix4x4 temp;


	temp.setIdentity();
	temp.setScale(Vec3(1.0f, 1.0f, 1.0f));
	worldcam *= temp;


	Vec3 new_pos = m_world_camera.getTranslation();
	float old_y = new_pos.y;



	Vec3 player_position;
	Vec3 player_dir;
	//since calling the actor manager will cause the player to initialize (which loads a lot of animation data and is slow)
	//we will only make the call if the camera has been set to a camera state which needs player data.
	if (m_cam_state == TP)
	{
		player_position = ActorManager::get()->getActivePlayerPosition();
		if(m_prev_state != m_cam_state) m_prev_player_pos = player_position;

		moveCamera(player_position - m_prev_player_pos);

		player_dir = ActorManager::get()->getActivePlayerDirection();
	}

	//declare variables necessary for camera update
	Vec3 cam_position = m_world_camera.getTranslation();

	Vec3 target_view = (player_position + Vec3(0, 3.0f, 0));
	Vec3 target_view_to_cam = cam_position - target_view;


	Vec3 temp_dir;
	float angle_to_player;
	float target_dist = 15.0f;
	float min_dist = 12.0f;
	Vec3 target_position;
	Vec2 mouse_delta = AppWindow::getMouseDelta();
	

	switch (m_cam_state)
	{
	case (FREE):
		new_pos = cam_position + m_world_camera.getZDirection() * (m_forward * m_speed);
		new_pos = new_pos + m_world_camera.getXDirection() * (m_rightward * m_speed);

		temp.setIdentity();
		temp.setRotationX(m_camera_rot.x);
		worldcam *= temp;

		temp.setIdentity();
		temp.setRotationY(m_camera_rot.y);
		worldcam *= temp;
		
		break;

	case (TP):
		new_pos = cam_position + m_world_camera.getYDirection() * (mouse_delta.x * m_speed * 50);
		new_pos = new_pos + m_world_camera.getXDirection() * (-mouse_delta.y * m_speed * 50);

		temp_dir = target_view - new_pos;
		temp_dir.normalize();

		new_pos = target_view - temp_dir * 10;

		worldcam.lookAt(target_view, new_pos, Vec3(0, -1, 0));
		break;

	case (TOPDOWN):

		new_pos = Vec3::lerp(new_pos, player_position + Vec3(0, 25.0f, -9.0f), delta * 10);

		temp.setIdentity();
		temp.setRotationX(70 * 0.01745f);
		worldcam *= temp;
		
		break;

	case (STATIC): 
		new_pos = cam_position;
		new_pos = new_pos;

		temp.setIdentity();
		temp.setRotationX(m_camera_rot.x);
		worldcam *= temp;

		temp.setIdentity();
		temp.setRotationY(m_camera_rot.y);
		worldcam *= temp;

		break;
	}

	if (exclude_Y_movement == true) new_pos.y = old_y;
	worldcam.setTranslation(new_pos);

	cc.m_camera_position = new_pos;
	m_world_camera = worldcam;
	worldcam.inverse();
	cc.m_view = worldcam;


	float depth = 2500.0f;
	cc.m_projection.setPerspectiveFovLH(1.0f, ((float)width / (float)height), 0.1f, depth);
	m_world_constant_buffer->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);

	//update the frustum for the current frame
	//錐台の更新
	m_frustum.constructFrustum(depth, cc.m_projection, cc.m_view);

	//save the current cam state for the next frame
	//前フレームのカメラステートを保存する
	m_prev_state = m_cam_state;
	//same for the player position
	//プレイヤーの位置も保存する
	m_prev_player_pos = player_position;
}

void CameraManager::beginLookAt(const Vec3& target, float seconds)
{
	//Matrix4x4 mat; 
	//mat.lookAt(target, m_world_camera.getTranslation(), Vector3D(0, 1, 0));
	//mat.setTranslation(m_world_camera.getTranslation());
	//m_world_camera = m_world_camera * (1.0f - seconds * delta) + mat * (seconds * delta);
	//if (seconds == 0) m_world_camera = mat;

	m_lookat = true;
	m_lookat_target = target;
	m_lookat_duration_until_complete = seconds;

}

void CameraManager::cancelLookAt()
{
	m_lookat = false;
	m_lookat_duration_until_complete = -1;
	m_lookat_target = {};
}

void CameraManager::beginMoveTo(Vec3 target, float seconds)
{
	//Vector3D pos = m_world_camera.getTranslation();
	//pos = pos * (1.0f - seconds * delta) + target * (seconds * delta);

	m_moveto = true;
	m_moveto_target = target;
	m_moveto_duration_until_complete = seconds;
}

void CameraManager::cancelMoveTo()
{
	m_moveto = false;
	m_moveto_duration_until_complete = -1;
	m_moveto_target = {};
}

void CameraManager::moveCamera(Vec3 move_amount)
{
	m_world_camera.setTranslation(m_world_camera.getTranslation() + move_amount);
}

void CameraManager::setWorldBuffer()
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPBufferVS(m_world_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPBufferGS(m_world_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPBufferDS(m_world_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPBufferPS(m_world_constant_buffer);
}

void CameraManager::setDirectionalLightWVPBuffer(Vec3 light_dir, const int& width, const int& height, const int& orthoID)
{
	cb_world_dirlight cc;
	cc.m_world.setIdentity();
	Matrix4x4 lightcam;
	Matrix4x4 temp;

	lightcam.setIdentity();
	temp.setIdentity();
	temp.setScale(Vec3(10.0f, 10.0f, 10.0f));
	lightcam *= temp;

	Vec3 persp = m_world_camera.getTranslation() + m_world_camera.getZDirection()*5 - light_dir * 30.0f;
	persp = m_world_camera.getTranslation() + light_dir * 30.0f;

	temp.setIdentity();
	temp.lookAt(m_world_camera.getTranslation() + m_world_camera.getZDirection() * 5, persp, Vec3(0, 1, 0));
	lightcam *= temp;

	lightcam.setTranslation(persp);
	lightcam.inverse();
	cc.m_view = lightcam;

	cc.m_projection[0].setOrthoLH(2.0f, 2.0f, 0.1f, 100.0f);
	cc.m_projection[1].setOrthoLH(5.0f, 5.0f, 0.1f, 100.0f);
	cc.m_projection[2].setOrthoLH(10.0f, 10.0f, 0.1f, 100.0f);
	
	cc.m_active_proj.m_x = orthoID;
	
	m_lightcam_constant_buffer->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPLightBufferVS(m_lightcam_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPLightBufferGS(m_lightcam_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPLightBufferDS(m_lightcam_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPLightBufferPS(m_lightcam_constant_buffer);
}

void CameraManager::createWorldBuffers()
{
	cb_world cc;
	cb_world_dirlight cc2;

	if (m_world_constant_buffer == nullptr)
	{
		m_world_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(cb_world));
	}

	if (m_lightcam_constant_buffer == nullptr)
	{
		m_lightcam_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc2, sizeof(cb_world_dirlight));
	}
}

bool CameraManager::isInFrustum_Point(Vec3 pos)
{
	return m_frustum.checkPoint(pos);
}

bool CameraManager::isInFrustum_Cube(Vec3 center, float radius)
{
	return m_frustum.checkCube(center, radius);
}

bool CameraManager::isInFrustum_Sphere(Vec3 pos, float radius)
{
	return m_frustum.checkSphere(pos, radius);
}

CameraManager::CameraManager()
{
	//camera settings
	m_world_camera.setTranslation(Vec3(0, 1, -5));

	m_cam_state = FREE;
	m_camera_rot = Vec2(0, 0);
}

void CameraManager::updateInput()
{
	if (m_cam_state == STATIC) return;

	m_forward = 0.0f;
	m_rightward = 0.0f;

	if (AppWindow::getKeyState('W'))
	{
		m_forward = 1.0f;
	}
	if (AppWindow::getKeyState('S'))
	{
		m_forward = -1.0f;
	}
	if (AppWindow::getKeyState('D'))
	{
		m_rightward = 1.0f;
	}
	if (AppWindow::getKeyState('A'))
	{
		m_rightward = -1.0f;
	}

	m_camera_rot = m_camera_rot + AppWindow::getMouseDelta();
	if (m_camera_rot.x < -1.57f) m_camera_rot.x = -1.57f;
}

void CameraManager::moveTo(float delta)
{
	Vec3 pos = m_world_camera.getTranslation();
	pos = pos * (1.0f - m_moveto_duration_until_complete * delta) 
		+ m_moveto_target * (m_moveto_duration_until_complete * delta);
	m_world_camera.setTranslation(pos);

	if (m_moveto_duration_until_complete < 0.0001f) m_world_camera.setTranslation(m_moveto_target);

	if ((m_world_camera.getTranslation() - m_moveto_target).length() < 0.001f)
	{
		m_moveto = false;
		m_moveto_duration_until_complete = -1;
		m_moveto_target = {};
	}
}

void CameraManager::lookAt(float delta)
{
	Matrix4x4 mat;
	mat.lookAt(m_lookat_target, m_world_camera.getTranslation(), Vec3(0, 1, 0));
	mat.setTranslation(m_world_camera.getTranslation());
	m_world_camera = m_world_camera * (1.0f - m_lookat_duration_until_complete * delta)
		+ mat * (m_lookat_duration_until_complete * delta);

	if (m_lookat_duration_until_complete < 0.0001f) m_world_camera = mat;

	if (Vec3::dot(m_world_camera.getZDirection(), mat.getZDirection()) < -0.999f )
	{
		m_lookat = false;
		m_lookat_duration_until_complete = -1;
		m_lookat_target = {};
	}
}
