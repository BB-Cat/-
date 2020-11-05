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
	Vector3D m_scale;
	Vector3D m_pos;
	Vector3D m_rot;
	Vector3D m_inertia;
	bool m_is_physics_object; //boolian which determines if the particle can collide with objects.  not used for now.
protected:
	friend class Emitter;
};