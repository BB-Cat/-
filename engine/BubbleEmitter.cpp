#include "BubbleEmitter.h"
#include "SP_Bubble.h"
#include "ParticlePrerequisites.h"
#include <d3d11.h>

BubbleEmitter::BubbleEmitter(int num_particles, Vec3 pos, float radius)
{
	m_max_particles = num_particles;
	m_pos = pos;
	m_radius = radius;
	m_is_active = true;
}

void BubbleEmitter::update(float delta)
{
	if (m_parts.size() < m_max_particles)
	{
		while(rand() % 3 == 0 && m_parts.size() < m_max_particles) spawn();
	}

	int ctr = 0; //counter of particles that are finished

	for (std::vector<SpriteParticle*>::iterator it = m_parts.begin(); it != m_parts.end(); ++it)
	{
		(*it)->update(delta);
		if (!(*it)->isAlive()) ctr++;
	}

	if (ctr == m_max_particles) m_is_active = false; //signal the particle system that this emitter is complete
}

void BubbleEmitter::spawn()
{
	Vec3 p_pos = m_pos + Vec3(
		m_radius * ((float)(rand() % 20) / 20.0f - 0.5f), 
		m_radius * ((float)(rand() % 20) / 20.0f - 0.5f), 
		m_radius * ((float)(rand() % 20) / 20.0f - 0.5f));

	float size = BUBBLE_MAX_SIZE - ((float)(rand() % 50) / 50.0f) * BUBBLE_SIZE_VARIATION;
	Vec3 scale = Vec3(size, size, size);

	float time = BUBBLE_DURATION - ((float)(rand() % 50) / 50.0f) * BUBBLE_DURATION_VARIATION;

	SpriteParticle* p = new SP_Bubble(&particle_sprites[Bubble], time,
		p_pos, scale, Vec3(p_pos - m_pos), Vec3(0, 0, 0));

	m_parts.push_back(p);
}
