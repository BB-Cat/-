#include "SpriteParticles.h"
#include "GraphicsEngine.h"

SpriteParticle::SpriteParticle(const wchar_t* spritefile, float duration, Vec3 position,
	Vec3 scale, Vec3 inertia, Vec3 rotation) : Particle()
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

void SpriteParticle::render(const Vec3& camera_dir, Vec2 tex_pos, Vec2 tex_size, Vec2 tex_origin)
{
	if (m_duration <= 0.0f) return;

	Vec3 dir_to_camera = camera_dir;
	dir_to_camera = dir_to_camera * -1; 

	float angle_to_cameraX = atan2(dir_to_camera.y, dir_to_camera.z);
	float angle_to_cameraY = atan2(dir_to_camera.x, dir_to_camera.z);

	m_sprite->renderWorldSpaceSprite(m_scale, m_pos, Vec3(angle_to_cameraX, angle_to_cameraY, 0),
		tex_pos, tex_size, tex_origin);
}
