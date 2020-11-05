#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "high_resolution_timer.h"

class Capsule
{
public:
	Capsule(Vector3D pos, float scale);
	~Capsule();
public:
	void update(high_resolution_timer delta, Terrain* terrain, Vector3D cap1, Vector3D cap2, float radius);
	void render(high_resolution_timer delta);

	bool checkCollisionCapsule(Vector3D cap1, Vector3D cap2, float radius);
	//get the shortest distance between two lines
	float calculateLineLineDistance(Vector3D p1, Vector3D e1, Vector3D p2, Vector3D e2, float& dist_from_l1, float& dist_from_l2);
	float calculateSegmentSegmentDistance(Vector3D p1, Vector3D e1, Vector3D p2, Vector3D e2);
	float calculatePointPointDistance(Vector3D point1, Vector3D point2);
	float calculatePointLineDistance(Vector3D point1, Vector3D line_p1, Vector3D line_p2, float &dist_from_l1);
	float calculatePointSegmentDistance(Vector3D point1, Vector3D line_p1, Vector3D line_p2, float& dist_from_l1);
	//calculate whether the angle between two vectors is acute
	bool isAcute(const Vector3D& v1, const Vector3D& v2);

public: //getters
	Vector3D getPosition() { return m_pos; }
	Vector3D getDirectionVector() { return Vector3D(sinf(m_angleY * 0.01745f), 0, cosf(m_angleY * 0.01745f)); };
	Vector3D getCapPos(bool bottom);
	float getRadius() { return m_radius; }
private:
	//calculate the position of the cap of the capsule
	Vector3D calculateCapPos(bool bottom);

private: //rendering members
	SkinnedMeshPtr m_mesh;
	Vector3D m_pos;
	Vector3D m_cap1, m_cap2;
	//Vector3D m_rot;
	Vector3D m_scale;
	float m_angleY, m_angleX;
	Vector3D m_color;

	float m_cylinder_len;
	float m_radius;
};