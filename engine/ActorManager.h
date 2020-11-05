#pragma once
#include "Prerequisites.h"
#include "Actor.h"
#include "Player.h"
#include "Enemy.h"
#include "Powerup.h"
#include "SkinnedMesh.h"
#include <vector>

class ActorManager
{
private:
	ActorManager();
	static ActorManager* am;
public:
	static ActorManager* get();
	~ActorManager();

	//clear all enemies and reset the player
	void initActors(); 
	
	void updateActors(float delta, Terrain* terrain);
	void renderActors(float delta);

private:
	PlayerPtr m_player;
	std::vector<EnemyPtr> m_enemies;
	std::vector<PowerUpPtr> m_powerups;
};