#include "Enemy.h"
#include "GraphicsEngine.h"
#include "Terrain.h"
#include "ActorManager.h"
//#include "Bullet.h"
#include "MyAudio.h"

Enemy::Enemy(Vec3 pos, float scale, Vec3 rot)
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

Enemy1::Enemy1(Vec3 pos, float scale, Vec3 rot, bool is_giant): Enemy(pos, scale, rot)
{
}

void Enemy1::update(float delta, Terrain* terrain)
{
}

Enemy2::Enemy2(Vec3 pos, float scale, Vec3 rot, bool is_giant) : Enemy(pos, scale, rot)
{
}

void Enemy2::update(float delta, Terrain* terrain)
{
}

Enemy3::Enemy3(Vec3 pos, float scale, Vec3 rot, bool is_giant) : Enemy(pos, scale, rot)
{
}

void Enemy3::render(float delta)
{
}

void Enemy3::update(float delta, Terrain* terrain)
{
}
