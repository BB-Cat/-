#pragma once
#include "Actor.h"
#include "Prerequisites.h"

class PowerUp : public Actor
{
public:
	PowerUp(Vec3 pos);
	~PowerUp();
	void update();
	void render(float delta);
public:
	bool checkPowerUpHit(Vec3 pos);

private:
	SkinnedMeshPtr m_mesh;
	int m_lightID;
	int m_timer;
	float m_alpha;
private:
	friend class ActorManager;
};