#pragma once
#include "Prerequisites.h"
#include <vector>
#include "Matrix4X4.h"
#include "ConstantBufferFormats.h"
#include "MyConstantBuffer.h"
#include "Vector2D.h"
#include "Frustum.h"

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
	static void release();
	
	void update(const float& delta, bool exclude_Y_movement = false);
	void beginLookAt(const Vec3& target, float seconds);
	void cancelLookAt();
	void beginMoveTo(Vec3 target, float seconds);
	void cancelMoveTo();
	void moveCamera(Vec3 move_amount);


	void createWorldBuffers();
	void setWorldBuffer();
	//orthoID determines which projection to activate for the next shadowmap pass
	void setDirectionalLightWVPBuffer(Vec3 light_dir, const int& orthoID);
	void setSpeed(float s) { m_speed = s; }

public:
	Matrix4x4 getCamera() { return m_world_camera; }
	void setCamState(int state) { m_cam_state = state; }
	void setCamPos(Vec3 pos) { m_world_camera.setTranslation(pos); }
	void setCamRot(Vec2 rot) { m_camera_rot = rot; }

public:
	// Culling Detection Functions //
	bool isInFrustum_Point(Vec3 pos);
	bool isInFrustum_Cube(Vec3 center, float radius);
	bool isInFrustum_Sphere(Vec3 center, float radius);

private:
	static CameraManager* instance;
	CameraManager();
	void updateInput();

	void moveTo(float delta);
	void lookAt(float delta);

private:
	Matrix4x4 m_world_camera;
	int m_cam_state;
	int m_prev_state;
	MyConstantBufferPtr m_world_constant_buffer;
	MyConstantBufferPtr m_lightcam_constant_buffer;

	Vec2 m_camera_rot;
	float m_forward = 0.0f;
	float m_rightward = 0.0f;
	float m_speed = 0.35f;

	//frustum class used to calculate culling
	Frustum m_frustum;

	//third person camera variables
	Vec3 m_prev_player_pos;

private:
	bool m_lookat;
	Vec3 m_lookat_target;
	float m_lookat_duration_until_complete;

private:
	bool m_moveto;
	Vec3 m_moveto_target;
	float m_moveto_duration_until_complete;
};