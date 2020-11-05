#pragma once
#include "SpriteParticles.h"

class SP_Bubble : public SpriteParticle
{
public:
	SP_Bubble(const wchar_t* spritefile, float duration, Vector3D position, Vector3D scale, Vector3D inertia, Vector3D rotation);
	~SP_Bubble();

	// SpriteParticle ����Čp������܂���
	virtual void update(float delta) override;
};
