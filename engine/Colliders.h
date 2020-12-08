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
	Vector3D getBoundingBox() { return m_bounding_box; }
	Vector3D getOffset() { return m_offset; }
	void setBoundingBox(Vector3D vec) { m_bounding_box = vec; }
	void setOffset(Vector3D vec) { m_offset = vec; }

protected:
	int m_type = -1;
	//size of the full bounding box around the primitive
	Vector3D m_bounding_box;
	Vector3D m_offset;
};


class CubeCollider : public Collider
{
public:
	CubeCollider(Vector3D scale) : Collider(ColliderTypes::Cube), m_scale(scale) 
	{
		m_bounding_box = scale;
	}
	CubeCollider(Collider col) : Collider(ColliderTypes::Cube), m_scale(col.getBoundingBox())
	{
		m_bounding_box = m_scale;
	}
	~CubeCollider() {}

	Vector3D getScale() { return m_scale; }

private:
	Vector3D m_scale;
};

class SphereCollider : public Collider
{
public:
	SphereCollider(float radius) : Collider(ColliderTypes::Sphere), m_radius(radius)
	{
		m_bounding_box = Vector3D(radius, radius, radius) * 2;
	}
	~SphereCollider() {}

	float getRadius() { return m_radius; }

private:
	float m_radius;
};