#include "SP_Bubble.h"



SP_Bubble::SP_Bubble(const wchar_t* spritefile, float duration, Vec3 position, Vec3 scale, Vec3 inertia, Vec3 rotation)
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
