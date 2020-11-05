#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include "SkinnedMesh.h"

class Actor
{
public:
	Vector3D getPosition() { return m_pos; };
	Vector3D getDirectionVector() { return Vector3D(sinf(m_angle * 0.01745f), 0, cosf(m_angle * 0.01745f)); };
protected:
	Vector3D m_pos;
	Vector3D m_rot;
	Vector3D m_scale;
	float m_angle;
	SkinnedMeshPtr m_mesh;
};
