#include "CameraManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "AppWindow.h"

CameraManager* CameraManager::cm = nullptr;

CameraManager* CameraManager::get()
{
	if (cm == nullptr) cm = new CameraManager();
	return cm;
}

CameraManager::~CameraManager()
{

}

void CameraManager::update(const float& delta, const int& width, const int& height, Vector3D* player_pos, Vector3D* player_rot)
{
	updateInput();

	Matrix4x4 worldcam;
	worldcam.setIdentity();
	cb_world cc;
	cc.m_world.setIdentity();
	Matrix4x4 temp;


	temp.setIdentity();
	temp.setScale(Vector3D(1.0f, 1.0f, 1.0f));
	worldcam *= temp;

	Vector3D new_pos = m_world_camera.getTranslation();

	//declare variables necessary for camera update
	Vector3D p_pos; 
	Vector3D p_dir; 
	Vector3D tempDir;
	float angle_to_player;

	switch (m_cam_state)
	{
	case (FREE):
			new_pos = m_world_camera.getTranslation() + m_world_camera.getZDirection() * (m_forward * m_speed);
			new_pos = new_pos + m_world_camera.getXDirection() * (m_rightward * m_speed);

			temp.setIdentity();
			temp.setRotationX(m_camera_rot.m_x);
			worldcam *= temp;

			temp.setIdentity();
			temp.setRotationY(m_camera_rot.m_y);
			worldcam *= temp;


			
			break;

	case (TP):

			if (player_pos == nullptr || player_rot == nullptr)
			{
				//throw std::exception("No player data was passed to the camera!");
				m_cam_state++;
				break;
			}
			p_pos = *player_pos;
			p_dir = *player_rot;

			new_pos = Vector3D::lerp(new_pos, p_pos - (p_dir * 3.0f) + Vector3D(0.5f, 0.75f, 0), delta * 10);

			temp.setIdentity();
			temp.setRotationX(5.0f * 0.01745f);
			worldcam *= temp;

			tempDir = (p_pos - new_pos);
			tempDir.normalize();
			angle_to_player = atan2(tempDir.m_x, tempDir.m_z);
			temp.setIdentity();
			temp.setRotationY(angle_to_player);
			worldcam *= temp;
		
			break;

	case (TOPDOWN):

			if (player_pos == nullptr)
			{
				//throw std::exception("No player data was passed to the camera!");
				m_cam_state++;
				break;
			}
			p_pos = *player_pos;

			new_pos = Vector3D::lerp(new_pos, p_pos + Vector3D(0, 25.0f, -9.0f), delta * 10);

			temp.setIdentity();
			temp.setRotationX(70 * 0.01745f);
			worldcam *= temp;
		
			break;

	case (STATIC): 
			break;

	}

	worldcam.setTranslation(new_pos);

	cc.m_camera_position = new_pos;
	m_world_camera = worldcam;
	worldcam.inverse();
	cc.m_view = worldcam;


	float depth = 2500.0f;
	cc.m_projection.setPerspectiveFovLH(1.0f, ((float)width / (float)height), 0.1f, depth);
	//cc.m_projection.setOrthoLH(1.0f, 1.0f, 0.1f, 100.0f);
	m_world_constant_buffer->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);

	//update the frustum for the current frame
	m_frustum.constructFrustum(depth, cc.m_projection, cc.m_view);
}

void CameraManager::setWorldBuffer()
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPBufferVS(m_world_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPBufferGS(m_world_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPBufferDS(m_world_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantWVPBufferPS(m_world_constant_buffer);
}

void CameraManager::setDirectionalLightWVPBuffer(Vector3D light_dir, const int& width, const int& height, const int& orthoID)
{
	cb_world_dirlight cc;
	cc.m_world.setIdentity();
	Matrix4x4 lightcam;
	Matrix4x4 temp;

	lightcam.setIdentity();
	temp.setIdentity();
	temp.setScale(Vector3D(10.0f, 10.0f, 10.0f));
	lightcam *= temp;

	Vector3D persp = m_world_camera.getTranslation() + m_world_camera.getZDirection()*5 -light_dir * 30.0f;


	temp.setIdentity();
	temp.lookAt(m_world_camera.getTranslation() + m_world_camera.getZDirection() * 5, persp, Vector3D(0, 1, 0));
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

bool CameraManager::isInFrustum_Point(Vector3D pos)
{
	return m_frustum.checkPoint(pos);
}

bool CameraManager::isInFrustum_Cube(Vector3D center, float radius)
{
	return m_frustum.checkCube(center, radius);
}

bool CameraManager::isInFrustum_Sphere(Vector3D pos, float radius)
{
	return m_frustum.checkSphere(pos, radius);
}

CameraManager::CameraManager()
{
	//camera settings
	m_world_camera.setTranslation(Vector3D(0, 1, -5));

	m_cam_state = FREE;
	m_camera_rot = Vector2D(0, 0);
}

void CameraManager::updateInput()
{

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

	if (AppWindow::getKeyTrigger('P'))
	{
		m_cam_state++;
		if (m_cam_state == CAMERA_STATE::MAX) m_cam_state = 0;
	}
}
