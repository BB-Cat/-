#pragma once
#include "ParticleSystem.h"

//particle class which uses a 2D particle texture that faces the camera
class SpriteParticle : public Particle
{
public:
	SpriteParticle(const wchar_t* spritefile, float duration, Vector3D position, Vector3D scale, Vector3D inertia, Vector3D rotation);
	~SpriteParticle();
	// Particle ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override = 0;
	virtual void render(const Vector3D& camera_dir, Vector2D tex_pos, Vector2D tex_size, Vector2D tex_origin);

private:
	SpritePtr m_sprite;
};