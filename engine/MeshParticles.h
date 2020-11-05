#pragma once
#include "ParticleSystem.h"

//particle which uses an actual 3D model for effects
class MeshParticle : public Particle
{
public:
	MeshParticle();
	~MeshParticle();
	// Particle ����Čp������܂���
	virtual void update(float delta) override = 0;


private:
	SkinnedMeshPtr m_mesh;
};