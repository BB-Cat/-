#include "Enemy.h"
#include "GraphicsEngine.h"
#include "Terrain.h"
#include "ActorManager.h"
//#include "Bullet.h"
#include "MyAudio.h"

Enemy::Enemy(Vector3D pos, float scale, Vector3D rot)
{
}

Enemy::~Enemy()
{
}

void Enemy::update(float delta, Terrain* terrain)
{
}

void Enemy::render(float delta)
{
}

Enemy1::Enemy1(Vector3D pos, float scale, Vector3D rot, bool is_giant): Enemy(pos, scale, rot)
{
}

void Enemy1::update(float delta, Terrain* terrain)
{
}

Enemy2::Enemy2(Vector3D pos, float scale, Vector3D rot, bool is_giant) : Enemy(pos, scale, rot)
{
}

void Enemy2::update(float delta, Terrain* terrain)
{
}

Enemy3::Enemy3(Vector3D pos, float scale, Vector3D rot, bool is_giant) : Enemy(pos, scale, rot)
{
}

void Enemy3::render(float delta)
{
}

void Enemy3::update(float delta, Terrain* terrain)
{
}
