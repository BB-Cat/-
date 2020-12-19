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
class CapsuleCollider;


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

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider(float radius, float core_length, Vec3 rot = Vec3(0)) : Collider(ColliderTypes::Capsule), 
		m_radius(radius), m_core_length(core_length), m_rotation(rot), m_was_updated(false)
	{
		calcCapPositions();
	}
	~CapsuleCollider() {}


	Vec3 getBoundingBox() override;
	//! For capsules this function is NOT IMPLEMENTED!! 
	//! Please use the other setters for capsule colliders after using reinterpret_cast<>()
	void setBoundingBox(Vec3 box) {}

	// Capsule Specific Functions -- these are very important for changing collider settings //

	float getRadius()		{ return m_radius; }
	float getCoreLength()	{ return m_core_length; }
	Vec3  getRotation()		{ return m_rotation; }
	void  getCaps(Vec3& cap1, Vec3& cap2);


	//NOTE - these setter functions will check to see if the new value is equal 
	//to the old value in order to avoid recalculating cap positions when possible
	void setRadius(float r) 
	{
		if (r == m_radius) return;
		m_radius = r; 
		m_was_updated = true;
	}
	void setCoreLength(float len) 
	{
		if (len == m_core_length) return;
		m_core_length = len; 
		m_was_updated = true;
	}
	void setRotation(Vec3 rot) 
	{
		if (rot == m_rotation) return;
		m_rotation = rot; 
		m_was_updated = true;

		//TODO: Fix the gimbal lock and coordinate system problem in the capsule collider!
		//
		m_rotation.x = 0;
		//
	}


private:
	//update the cap position data of the capsules, called automatically whenever data has been updated
	void calcCapPositions();

private:
	float m_radius;
	float m_core_length;
	Vec3  m_rotation;

	//saved cap location values.  updated whenever the other variables are changed;
	Vec3 m_cap1; 
	Vec3 m_cap2;
	bool m_was_updated;
};