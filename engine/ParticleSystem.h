#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include "Particle.h"
#include "Sprite.h"
#include "Emitter.h"
#include <vector>

enum Emitters
{
	BUBBLE_EMIT,

};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void update(float delta);
	void render();
	void createEmitter(int type, int particle_max, Vector3D pos, float radius);
	void deleteEmitter();

private:
	std::vector<Emitter*> m_emitters;
};