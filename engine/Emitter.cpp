#include "Emitter.h"
#include "Particle.h"
#include "SpriteParticles.h"
#include "CameraManager.h"

Emitter::Emitter()
{
}

Emitter::~Emitter()
{
	for (std::vector<SpriteParticle*>::iterator it = m_parts.begin(); it != m_parts.end(); ++it)
	{
		delete(*it);
	}

	m_parts.clear();
}

void Emitter::render()
{
	for (std::vector<SpriteParticle*>::iterator it = m_parts.begin(); it != m_parts.end(); ++it) 
	{
		//send the camera position to particles
		Matrix4x4 cam = CameraManager::get()->getCamera();
		Vec3 cam_dir = cam.getZDirection();

		(*it)->render(cam_dir, Vec2(0,0), Vec2(300,300), Vec2(150, 150));
	}
}
