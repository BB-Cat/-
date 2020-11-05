#pragma once
#include "Actor.h"
#include "Prerequisites.h"

class PowerUp : public Actor
{
public:
	PowerUp(Vector3D pos);
	~PowerUp();
	void update();
	void render(float delta);
public:
	bool checkPowerUpHit(Vector3D pos);

private:
	SkinnedMeshPtr m_mesh;
	int m_lightID;
	int m_timer;
	float m_alpha;
private:
	friend class ActorManager;
};