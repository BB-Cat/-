#include "ActorManager.h"
#include "GraphicsEngine.h"
#include "Bullet.h"
#include "MyAudio.h"

ActorManager* ActorManager::instance = nullptr;

ActorManager::ActorManager()
{
	m_actor_lists.push_back(Faction(L"Player"));
	m_actor_lists.push_back(Faction(L"Enemy"));
	m_actor_lists.push_back(Faction(L"Ambient"));

	PlayerPtr p = std::shared_ptr<Player>(new Player(true));
	getFaction(L"Player")->m_actors.push_back(p);
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

Vector3D ActorManager::getActivePlayerPosition()
{
	if (m_actor_lists[FactionID::PLAYER].m_actors.size() > m_active_player)
	{
		return m_actor_lists[FactionID::PLAYER].m_actors[m_active_player]->getPosition();
	}

	//プレイヤーが存在しない場合初期化Vector3Dを返す
	return Vector3D();
}

Vector3D ActorManager::getActivePlayerDirection()
{
	if (m_actor_lists[FactionID::PLAYER].m_actors.size() > m_active_player)
	{
		return m_actor_lists[FactionID::PLAYER].m_actors[m_active_player]->getDirectionVector();
	}

	//プレイヤーが存在しない場合初期化Vector3Dを返す
	return Vector3D();
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

