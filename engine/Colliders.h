#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"

namespace ColliderTypes
{
	enum 
	{
		None,
		Cube,
		Sphere,
		Capsule,
		MAX
	};
}

class Collider
{
public:
	Collider(int type) : m_type(type) {}
	~Collider() {}



	int getType() { return m_type; }

protected:
	int m_type = -1;
};


class CubeCollider : public Collider
{
public:
	CubeCollider(Vector3D scale) : Collider(ColliderTypes::Cube), m_scale(scale) {}
	~CubeCollider() {}

	Vector3D getScale() { return m_scale; }

private:
	Vector3D m_scale;
};