#pragma once
#include <vector>
#include "Particle.h"
#include "Vector3D.h"
//#include "ParticlePrerequisites.h"
#include "SpriteParticles.h"

class SpriteParticle;

//TODO : right now this emitter only works for 2D sprites.  it needs to be split into two classes for 3d and 2d
class Emitter
{
public:
	Emitter();
	~Emitter();

	virtual void update(float delta) = 0;
	void render();
	//depending on the emitter, the spawn function may or may not be called from outside the class
	virtual void spawn() = 0; 

	bool isActive() { return m_is_active; }

protected:
	std::vector<SpriteParticle*> m_parts;
	Vector3D m_pos; //position of the emitter in 3D space
	float m_radius; //radius where the emitter can spawn particles
	bool m_is_active;		//bool which tells the particle system to delete if it turns false
	int m_max_particles;	//integer which tracks the limit of particles this emitter is allowed to create
};
