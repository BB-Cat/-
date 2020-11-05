#include "Lighting.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "AppWindow.h"

Lighting* Lighting::l = nullptr;
int Lighting::m_next_light_id = 0;

Lighting::Lighting()
{
	m_sl.m_global_light_color = Vector3D(1.0f, 0.9f, 0.3f);
	m_sl.m_global_light_dir = Vector3D(0.05f, 1.0f, 0.05f);
	m_sl.m_global_light_strength = 1.0f;
	m_sl.m_ambient_light_color = Vector3D(0, 0, 0);
}

Lighting::~Lighting()
{
	m_lights.clear();
}

void Lighting::setLights()
{
	for (int i = 0; i < m_lights.size(); i++)
	{
		GraphicsEngine::get()->getConstantBufferSystem()->updateLocalLightPropertyBuffer(m_lights[i].m_pos, m_lights[i].m_color, m_lights[i].m_strength);
		if(m_lights[i].m_lifespan > 0) m_lights[i].m_lifespan--;
	}

	// clear the lights which finished their lifespans
	for (int i = 0; i < m_lights.size(); i++)
	{
		if (m_lights[i].m_lifespan == 0) m_lights.erase(m_lights.begin() + i);
	}

	//clear the lights which were queued for erasure
	for (int i = 0; i < m_lights.size(); i++)
	{
		if(m_lights[i].m_is_erase == true) m_lights.erase(m_lights.begin() + i);
	}
}

int Lighting::spawnLight(int type, int lifespan, Vector3D pos, Vector3D color, float strength)
{
	if (m_lights.size() >= NUM_LIGHTS) return 0;

	Light new_light(++m_next_light_id, lifespan, pos, color, strength);

	//switch (type)
	//{
	//case 0:
	//	break;
	//case 1:
	//	break;
	//}

	//new_light->m_id = ++m_next_light_id;
	//new_light->m_lifespan = lifespan;
	//new_light->m_max_lifespan = lifespan;
	//new_light->m_pos = pos;
	//new_light->m_color = color;
	//new_light->m_strength = strength;
	//new_light->m_orig_strength = strength;
	m_lights.push_back(new_light);
	return new_light.m_id;
}

void Lighting::clearLight(int id)
{
	for (int i = 0; i < m_lights.size(); i++)
	{
		if (m_lights[i].m_id == id)
		{
			//set the values for this light to zero so that the constant buffer is updated before deleting the light in the next setLights() call.
			m_lights[i].m_pos = Vector3D(0,0,0);
			m_lights[i].m_color = Vector3D(0, 0, 0);
			m_lights[i].m_strength = 0;
			m_lights[i].m_is_erase = true;
		}
	}
}



bool Lighting::updateLocalLight(const int& id, const Vector3D& pos, const Vector3D& color, const float& strength)
{
	for (int i = 0; i < m_lights.size(); i++)
	{
		if (m_lights[i].m_id == id)
		{
			m_lights[i].m_pos = pos;
			m_lights[i].m_color = color;
			m_lights[i].m_strength = strength;

			return true;
		}
	}
	
	return false;
}

void Lighting::updateSceneLight(const Vector3D& dir, const Vector3D& color, const float& strength, const Vector3D& ambient_color)
{
	m_sl.m_global_light_dir = dir;
	m_sl.m_global_light_color = color;
	m_sl.m_global_light_strength = strength;
	m_sl.m_ambient_light_color = ambient_color;

	GraphicsEngine::get()->getConstantBufferSystem()->updateGlobalLightPropertyBuffer(
		m_sl.m_global_light_dir, m_sl.m_global_light_strength, m_sl.m_global_light_color, m_sl.m_ambient_light_color);
}

void Lighting::sceneLightingInput(float delta)
{
	bool change = false;

	if (AppWindow::getKeyState('Z'))
	{
		m_sl.m_global_light_strength = min(m_sl.m_global_light_strength + 1.0f * delta, 1.0);
		change = true;
	}
	else if (AppWindow::getKeyState('X'))
	{
		m_sl.m_global_light_strength = max(m_sl.m_global_light_strength - 1.0f * delta, 0.0);
		change = true;
	}

	if (AppWindow::getKeyState('1'))
	{
		m_sl.m_global_light_color.m_x = min(m_sl.m_global_light_color.m_x + 1.0f * delta, 1.0);
		change = true;
	}
	else if (AppWindow::getKeyState('2'))
	{
		m_sl.m_global_light_color.m_x = max(m_sl.m_global_light_color.m_x - 1.0f * delta, 0.0);
		change = true;
	}

	if (AppWindow::getKeyState('3'))
	{
		m_sl.m_global_light_color.m_y = min(m_sl.m_global_light_color.m_y + 1.0f * delta, 1.0);
		change = true;
	}
	else if (AppWindow::getKeyState('4'))
	{
		m_sl.m_global_light_color.m_y = max(m_sl.m_global_light_color.m_y - 1.0f * delta, 0.0);
		change = true;
	}

	if (AppWindow::getKeyState('5'))
	{
		m_sl.m_global_light_color.m_z = min(m_sl.m_global_light_color.m_z + 1.0f * delta, 1.0);
		change = true;
	}
	else if (AppWindow::getKeyState('6'))
	{
		m_sl.m_global_light_color.m_z = max(m_sl.m_global_light_color.m_z - 1.0f * delta, 0.0);
		change = true;
	}

	if (change)
	{
		GraphicsEngine::get()->getConstantBufferSystem()->updateGlobalLightPropertyBuffer(
			m_sl.m_global_light_dir, m_sl.m_global_light_strength, m_sl.m_global_light_color, m_sl.m_ambient_light_color);
	}
}

