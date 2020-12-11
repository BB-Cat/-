#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"

//Enumeration for collider categories
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

//forward declarations
class CubeCollider;
class SphereCollider;


class Collider
{
public:
	Collider(int type) : m_type(type) {}
	~Collider() {}

	int getType() { return m_type; }

	//determine the bounding box of this collider's geometry and return the vector
	virtual Vec3 getBoundingBox();
	//change the internal variables of the inherited class depending on how that class defines its bounding box
	virtual void setBoundingBox(Vec3 box);

	//get the offset from the origin point of reference
	Vec3 getOffset() { return m_offset; }
	//set the offset from the origin point of reference
	void setOffset(Vec3 vec) { m_offset = vec; }

protected:
	int m_type = -1;
	Vec3 m_offset;
};


class CubeCollider : public Collider
{
public:
	CubeCollider(Vec3 scale) : Collider(ColliderTypes::Cube), m_scale(scale) 
	{
	}
	CubeCollider(Collider col) : Collider(ColliderTypes::Cube), m_scale(col.getBoundingBox())
	{
	}
	~CubeCollider() {}

	Vec3 getScale() { return m_scale; }
	Vec3 getBoundingBox() override { return m_scale; }
	void setBoundingBox(Vec3 box) override { m_scale = box; }

private:
	Vec3 m_scale;
};

class SphereCollider : public Collider
{
public:
	SphereCollider(float radius) : Collider(ColliderTypes::Sphere), m_radius(radius) {}
	~SphereCollider() {}

	float getRadius() { return m_radius; }
	Vec3 getBoundingBox() override { return Vec3(m_radius * 2.0f); }
	//! Please note that only the X component of the box is actually being read to decide the radius!
	void setBoundingBox(Vec3 box) override;

private:
	float m_radius;
};