#pragma once
#include "Actor.h"

#define ESHOT_SPEED		(15.0f)		//speed of enemy default bullets
#define ENEMY1_SPEED    (7.5f)
#define ENEMY2_SPEED    (9.5f)
#define ENEMY3_SPEED    (11.0f)

#define ENEMY_HURT_TIMER (10)

class Enemy : public Actor
{
public:
	Enemy(Vector3D pos, float scale = 1.0f, Vector3D rot = Vector3D(0,0,0));
	virtual ~Enemy();

	virtual void update(float delta, Terrain* terrain) = 0;
	virtual void render(float delta);

protected:
	SkinnedMeshPtr m_mesh;

protected:
	friend class ActorManager;
};

class Enemy1 : public Enemy //chases the player directly at a low speed
{
public:
	Enemy1(Vector3D pos, float scale = 1.0f, Vector3D rot = Vector3D(0, 0, 0), bool is_giant = false);

	virtual void update(float delta, Terrain* terrain) override;
private:

};

class Enemy2 : public Enemy //stops and occasionally shoots bullets at the player
{
public:
	Enemy2(Vector3D pos, float scale = 1.0f, Vector3D rot = Vector3D(0, 0, 0), bool is_giant = false);

	virtual void update(float delta, Terrain* terrain) override;
private:
};

class Enemy3 : public Enemy //stops and occasionally shoots bullets at the player
{
public:
	Enemy3(Vector3D pos, float scale = 1.0f, Vector3D rot = Vector3D(0, 0, 0), bool is_giant = false);
	virtual void render(float delta) override;

	virtual void update(float delta, Terrain* terrain) override;
private:
};