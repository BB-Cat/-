#include "ActorManager.h"
#include "GraphicsEngine.h"
#include "Player.h"
//#include "Bullet.h"
#include "MyAudio.h"

ActorManager* ActorManager::instance = nullptr;

ActorManager::ActorManager()
{
	m_actor_lists.push_back(Faction(L"Player"));
	m_actor_lists.push_back(Faction(L"Enemy"));
	m_actor_lists.push_back(Faction(L"Ambient"));

	m_player = std::shared_ptr<Player>(new Player(true));
	getFaction(L"Player")->m_actors.push_back(m_player);
}

ActorManager* ActorManager::get()
{
	if (instance == nullptr) instance = new ActorManager();
	return instance;
}

ActorManager::~ActorManager()
{
}

void ActorManager::updateAll(float delta)
{
	for (int i = 0; i < m_actor_lists.size(); i++)
	{
		for (int j = 0; j < m_actor_lists[i].m_actors.size(); j++)
		{
			m_actor_lists[i].m_actors[j]->update(delta);
		}
	}
}

void ActorManager::updateFaction(std::wstring name, float delta)
{
	for (int i = 0; i < m_actor_lists.size(); i++)
	{
		if (m_actor_lists[i].m_name == name)
		{
			for (int j = 0; j < m_actor_lists[i].m_actors.size(); j++)
			{
				m_actor_lists[i].m_actors[j]->update(delta);
			}
			break;
		}
	}
}

void ActorManager::renderAll(float delta)
{
	for (int i = 0; i < m_actor_lists.size(); i++)
	{
		for (int j = 0; j < m_actor_lists[i].m_actors.size(); j++)
		{
			m_actor_lists[i].m_actors[j]->render(delta);
		}
	}
}

void ActorManager::renderFaction(std::wstring name, float delta)
{
	for (int i = 0; i < m_actor_lists.size(); i++)
	{
		if (m_actor_lists[i].m_name == name)
		{
			for (int j = 0; j < m_actor_lists[i].m_actors.size(); j++)
			{
				m_actor_lists[i].m_actors[j]->render(delta);
			}
			break;
		}
	}
}

void ActorManager::renderShadowsAll(float delta)
{
	for (int i = 0; i < m_actor_lists.size(); i++)
	{
		for (int j = 0; j < m_actor_lists[i].m_actors.size(); j++)
		{
			if (m_actor_lists[i].m_actors[j]->m_has_shadow == false) continue;
			m_actor_lists[i].m_actors[j]->renderShadow(delta);
		}
	}
}

void ActorManager::renderFactionShadows(std::wstring name, float delta)
{
	for (int i = 0; i < m_actor_lists.size(); i++)
	{
		if (m_actor_lists[i].m_name == name)
		{
			for (int j = 0; j < m_actor_lists[i].m_actors.size(); j++)
			{
				if (m_actor_lists[i].m_actors[j]->m_has_shadow == false) continue;
				m_actor_lists[i].m_actors[j]->renderShadow(delta);
			}
			break;
		}
	}
}

int ActorManager::getActivePlayerState()
{
	if (m_player != nullptr)
	{
		return m_player->m_state;
	}
	return -1;
}

Vec3 ActorManager::getActivePlayerPosition()
{
	if (m_player != nullptr)
	{
		return m_player->m_pos;
	}

	//プレイヤーが存在しない場合初期化Vector3Dを返す
	return Vec3();
}

void ActorManager::setActivePlayerPosition(Vec3 pos)
{
	if (m_player != nullptr)
	{
		if (m_player->m_pos.x != pos.x || m_player->m_pos.z != pos.z)
		{
			int a = 3;
		}
		m_player->m_pos = pos;
		//m_player->m_pos = Vec3(0, 0, 0);
	}
}

void ActorManager::stopActivePlayerAscent()
{
	if (m_player != nullptr)
	{
		m_player->m_jump_speed = 0;
	}
}

void ActorManager::stopActivePlayerJump()
{
	if (m_player != nullptr)
	{
		m_player->endJump(0);
	}
}

void ActorManager::startActivePlayerFall()
{
	if (m_player != nullptr)
	{
		m_player->m_state = PlayerState::Jump;
		m_player->m_jump = true;
		m_player->m_jump_speed = 0;
	}
}

Vec3 ActorManager::getActivePlayerDirection()
{
	if (m_actor_lists[FactionID::PLAYER].m_actors.size() > m_active_player)
	{
		return m_actor_lists[FactionID::PLAYER].m_actors[m_active_player]->getDirectionVector();
	}

	//プレイヤーが存在しない場合初期化Vector3Dを返す
	return Vec3();
}

void ActorManager::activePlayerImGui()
{
	if (m_actor_lists[FactionID::PLAYER].m_actors.size() > m_active_player)
	{
		m_actor_lists[FactionID::PLAYER].m_actors[m_active_player]->imGuiWindow();
	}
}

Faction* ActorManager::getFaction(std::wstring faction_name)
{
	for (int i = 0; i < m_actor_lists.size(); i++)
	{
		//名前が参照と統一していればポインターを返す
		if (m_actor_lists[i].m_name == faction_name) return &m_actor_lists[i];
	}
	
	//アクターのカテゴリーが見つからなかった場合、ヌルポインターを返す
	return nullptr;
}

