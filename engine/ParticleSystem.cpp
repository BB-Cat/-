#include "ParticleSystem.h"
#include "GraphicsEngine.h"
#include "BubbleEmitter.h"

//TODO : primitive particle class!
//TODO : GPU particle implementation!

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
	for (std::vector<Emitter*>::iterator it = m_emitters.begin(); it != m_emitters.end(); ++it)
	{
		delete(*it);
	}

	m_emitters.clear();
}

void ParticleSystem::update(float delta)
{
	for (std::vector<Emitter*>::iterator it = m_emitters.begin(); it != m_emitters.end(); ++it)
	{
		(*it)->update(delta);
	}

	for (int i = 0; i < m_emitters.size(); i++)
	{
		if (!m_emitters[i]->isActive()) m_emitters.erase(m_emitters.begin() + i);
	}
}

void ParticleSystem::render()
{
	for (std::vector<Emitter*>::iterator it = m_emitters.begin(); it != m_emitters.end(); ++it)
	{
		(*it)->render();
	}
}

void ParticleSystem::createEmitter(int type, int particle_max, Vector3D pos, float radius)
{
	Emitter* e = nullptr;

	switch (type)
	{
	case BUBBLE_EMIT:
		e = new BubbleEmitter(particle_max, pos, radius);
		break;
	}

	m_emitters.push_back(e);
}

void ParticleSystem::deleteEmitter()
{

}
