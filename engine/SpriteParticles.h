#pragma once
#include "ParticleSystem.h"

//particle class which uses a 2D particle texture that faces the camera
class SpriteParticle : public Particle
{
public:
	SpriteParticle(const wchar_t* spritefile, float duration, Vec3 position, Vec3 scale, Vec3 inertia, Vec3 rotation);
	~SpriteParticle();
	// Particle ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override = 0;
	virtual void render(const Vec3& camera_dir, Vec2 tex_pos, Vec2 tex_size, Vec2 tex_origin);

private:
	SpritePtr m_sprite;
};