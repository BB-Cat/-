#pragma once
#include "Vector3D.h"

//base class for particles 
class Particle
{
public:
	Particle() {}
	~Particle() {}
	virtual void update(float delta) {}
	//virtual void render(const Vector3D& camera_pos) {}

	bool isAlive() { return (m_duration > 0.0f); }

protected:
	float m_duration;
	float m_max_duration;
	Vec3 m_scale;
	Vec3 m_pos;
	Vec3 m_rot;
	Vec3 m_inertia;
	bool m_is_physics_object; //boolian which determines if the particle can collide with objects.  not used for now.
protected:
	friend class Emitter;
};