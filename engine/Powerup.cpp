#include "Powerup.h"
#include "SkinnedMesh.h"
#include "GraphicsEngine.h"
#include "Lighting.h"
#include "ActorManager.h"
#include "MyAudio.h"

PowerUp::PowerUp(Vector3D pos)
{
	m_pos = pos;

	m_mesh = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\OBJ\\powerup.fbx", true, nullptr);

	m_scale = Vector3D(1, 1, 1);
	m_rot = Vector3D(0, 0, 0);
	m_angle = 0.0f;


	m_lightID = Lighting::get()->spawnLight(0, -1, m_pos, Vector3D(1, 1, 1), 1.0f);
	m_timer = 1000;
}

PowerUp::~PowerUp()
{
	//ActorManager::get()->powerupPlayer(1);
	Lighting::get()->clearLight(m_lightID);
}

void PowerUp::update()
{
	m_angle += 2.0f;
	m_alpha = min(1.0f, (float)m_timer / 100.00f);
	m_timer--;
}

void PowerUp::render(float delta)
{
	//m_mesh->renderMesh(delta, m_scale, m_pos, Vector3D(0, m_angle * 0.01745f, 0), Vector4D(1.0f, 1.0f, 1.0f, m_alpha));
}

bool PowerUp::checkPowerUpHit(Vector3D pos)
{
	return ((m_pos - pos).length() < 1.0f);
}

