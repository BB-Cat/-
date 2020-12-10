#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "high_resolution_timer.h"

class Capsule
{
public:
	Capsule(Vec3 pos, float scale);
	~Capsule();
public:
	void update(high_resolution_timer delta, Terrain* terrain, Vec3 cap1, Vec3 cap2, float radius);
	void render(high_resolution_timer delta);

	bool checkCollisionCapsule(Vec3 cap1, Vec3 cap2, float radius);
	//get the shortest distance between two lines
	float calculateLineLineDistance(Vec3 p1, Vec3 e1, Vec3 p2, Vec3 e2, float& dist_from_l1, float& dist_from_l2);
	float calculateSegmentSegmentDistance(Vec3 p1, Vec3 e1, Vec3 p2, Vec3 e2);
	float calculatePointPointDistance(Vec3 point1, Vec3 point2);
	float calculatePointLineDistance(Vec3 point1, Vec3 line_p1, Vec3 line_p2, float &dist_from_l1);
	float calculatePointSegmentDistance(Vec3 point1, Vec3 line_p1, Vec3 line_p2, float& dist_from_l1);
	//calculate whether the angle between two vectors is acute
	bool isAcute(const Vec3& v1, const Vec3& v2);

public: //getters
	Vec3 getPosition() { return m_pos; }
	Vec3 getDirectionVector() { return Vec3(sinf(m_angleY * 0.01745f), 0, cosf(m_angleY * 0.01745f)); };
	Vec3 getCapPos(bool bottom);
	float getRadius() { return m_radius; }
private:
	//calculate the position of the cap of the capsule
	Vec3 calculateCapPos(bool bottom);

private: //rendering members
	SkinnedMeshPtr m_mesh;
	Vec3 m_pos;
	Vec3 m_cap1, m_cap2;
	//Vector3D m_rot;
	Vec3 m_scale;
	float m_angleY, m_angleX;
	Vec3 m_color;

	float m_cylinder_len;
	float m_radius;
};