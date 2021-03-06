#pragma once
#include "ParticleSystem.h"

//particle which uses an actual 3D model for effects
class MeshParticle : public Particle
{
public:
	MeshParticle();
	~MeshParticle();
	// Particle を介して継承されました
	virtual void update(float delta) override = 0;


private:
	SkinnedMeshPtr m_mesh;
};