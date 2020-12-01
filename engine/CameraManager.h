#pragma once
#include "Prerequisites.h"
#include <vector>
#include "Matrix4X4.h"
#include "ConstantBufferFormats.h"
#include "MyConstantBuffer.h"
#include "Vector2D.h"
#include "Frustum.h"

//TODO: for now i have simply moved the camera updating code to the camera manager singleton class.  later I will make a stack which 
//pulls data from whatever camera is on top, allowing me to dynamically switch the active camera, use transitions, and orchestrate cutscenes more easily.


enum CAMERA_STATE
{
	STATIC, //immovable camera
	FREE,   //free movement in three directions
	TP,		//third person camera which follows a target position and rotation
	TOPDOWN, //third person camera which views a target from above and chases their xy position
	MAX,    //unused value which tells the camera state to reset
};


class CameraManager
{
public:
	static CameraManager* get();
	~CameraManager();
	
	void update(const float& delta, const int& width, const int& height, bool exclude_Y_movement = false);
	void beginLookAt(const Vector3D& target, float seconds);
	void cancelLookAt();
	void beginMoveTo(Vector3D target, float seconds);
	void cancelMoveTo();
	void moveCamera(Vector3D move_amount);


	void createWorldBuffers();
	void setWorldBuffer();
	//orthoID determines which projection to activate for the next shadowmap pass
	void setDirectionalLightWVPBuffer(Vector3D light_dir, const int& width, const int& height, const int& orthoID);
	void setSpeed(float s) { m_speed = s; }

public:
	Matrix4x4 getCamera() { return m_world_camera; }
	void setCamState(int state) { m_cam_state = state; }
	void setCamPos(Vector3D pos) { m_world_camera.setTranslation(pos); }
	void setCamRot(Vector2D rot) { m_camera_rot = rot; }

public:
	// Culling Detection Functions //
	bool isInFrustum_Point(Vector3D pos);
	bool isInFrustum_Cube(Vector3D center, float radius);
	bool isInFrustum_Sphere(Vector3D center, float radius);

private:
	static CameraManager* cm;
	CameraManager();
	void updateInput();

	void moveTo(float delta);
	void lookAt(float delta);

private:
	Matrix4x4 m_world_camera;
	int m_cam_state;
	MyConstantBufferPtr m_world_constant_buffer;
	MyConstantBufferPtr m_lightcam_constant_buffer;

	Vector2D m_camera_rot;
	float m_forward = 0.0f;
	float m_rightward = 0.0f;
	float m_speed = 0.35f;

	//frustum class used to calculate culling
	Frustum m_frustum;

private:
	bool m_lookat;
	Vector3D m_lookat_target;
	float m_lookat_duration_until_complete;

private:
	bool m_moveto;
	Vector3D m_moveto_target;
	float m_moveto_duration_until_complete;
};