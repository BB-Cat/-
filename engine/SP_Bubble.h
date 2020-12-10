#pragma once
#include "SpriteParticles.h"

class SP_Bubble : public SpriteParticle
{
public:
	SP_Bubble(const wchar_t* spritefile, float duration, Vec3 position, Vec3 scale, Vec3 inertia, Vec3 rotation);
	~SP_Bubble();

	// SpriteParticle ‚ğ‰î‚µ‚ÄŒp³‚³‚ê‚Ü‚µ‚½
	virtual void update(float delta) override;
};
