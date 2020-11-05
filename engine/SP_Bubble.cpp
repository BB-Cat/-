#include "SP_Bubble.h"



SP_Bubble::SP_Bubble(const wchar_t* spritefile, float duration, Vector3D position, Vector3D scale, Vector3D inertia, Vector3D rotation)
	: SpriteParticle(spritefile, duration, position, scale, inertia, rotation)
{
}

SP_Bubble::~SP_Bubble()
{
}

void SP_Bubble::update(float delta)
{
	m_duration -= delta;
}
