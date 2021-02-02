#pragma once
#include "Prerequisites.h"
#include "Colliders.h"

class AnmHitbox
{
public:
	AnmHitbox(float* active_frames, std::shared_ptr<Collider> collider, int damage = 0)
	{
		m_active_frames[0] = active_frames[0];
		m_active_frames[1] = active_frames[1];

		m_collider = collider;

		m_damage_value = damage;
	}
	~AnmHitbox() 
	{
	}

	//check whether or not the hitbox is live at the selected frame
	bool isActiveAtFrame(int frame) { return (frame >= m_active_frames[0] && frame <= m_active_frames[1]); }

	int m_active_frames[2] = {};
	int m_damage_value = 0;
	std::shared_ptr<Collider> m_collider = nullptr;
};

