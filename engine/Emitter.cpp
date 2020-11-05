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
		Vector3D cam_dir = cam.getZDirection();

		(*it)->render(cam_dir, Vector2D(0,0), Vector2D(300,300), Vector2D(150, 150));
	}
}
