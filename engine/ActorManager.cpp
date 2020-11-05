#include "ActorManager.h"
#include "GraphicsEngine.h"
#include "Bullet.h"
#include "MyAudio.h"

ActorManager* ActorManager::am = nullptr;

ActorManager::ActorManager()
{
}

ActorManager* ActorManager::get()
{
	if (am == nullptr) am = new ActorManager();
	return am;
}

ActorManager::~ActorManager()
{
}

void ActorManager::initActors()
{
}

void ActorManager::updateActors(float delta, Terrain* terrain)
{

}

void ActorManager::renderActors(float delta)
{
}

