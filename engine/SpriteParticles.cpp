#include "SpriteParticles.h"
#include "GraphicsEngine.h"

SpriteParticle::SpriteParticle(const wchar_t* spritefile, float duration, Vector3D position,
	Vector3D scale, Vector3D inertia, Vector3D rotation) : Particle()
{
	m_sprite = GraphicsEngine::get()->getSpriteManager()->createSpriteFromFile(spritefile);
	if (m_sprite == nullptr)
		throw std::exception("Could not initialize sprite texture in the SpriteParticle!");

	m_duration = duration;
	m_max_duration = duration;

	m_pos = position;
	m_scale = scale;
	m_inertia = inertia;
	m_rot = rotation;

	m_is_physics_object = false;
}

SpriteParticle::~SpriteParticle()
{
}

void SpriteParticle::render(const Vector3D& camera_dir, Vector2D tex_pos, Vector2D tex_size, Vector2D tex_origin)
{
	if (m_duration <= 0.0f) return;

	Vector3D dir_to_camera = camera_dir;
	dir_to_camera = dir_to_camera * -1; 

	float angle_to_cameraX = atan2(dir_to_camera.m_y, dir_to_camera.m_z);
	float angle_to_cameraY = atan2(dir_to_camera.m_x, dir_to_camera.m_z);

	m_sprite->renderSprite(m_scale, m_pos, Vector3D(angle_to_cameraX, angle_to_cameraY, 0),
		tex_pos, tex_size, tex_origin);
}
