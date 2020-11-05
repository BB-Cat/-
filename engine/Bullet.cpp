#include "Bullet.h"
#include "StaticMeshManager.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "Lighting.h"


//mesh index for bullet types (ineffective but good enough for now)
int MeshIndex[E_MAX] =
{
	0,  //pdefault 
	0,  //pmachine
	0,  //pspread
	0,	//curve
	0,	//bomb
	0,	//explosiion
	-1, //pmax placeholder
	1,  //edefault
	1,  //egiant
};



Bullet::~Bullet()
{
	if (m_light_id) Lighting::get()->clearLight(m_light_id);
}

void Bullet::init(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	m_lifespan		= life;
	m_max_lifespan  = life;
	m_speed			= speed;
	m_pos			= pos;
	m_ricochet		= ricochet;
	m_weight		= weight;
	m_color			= color;
}


PlayerShot_Default::PlayerShot_Default(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	m_type = P_DEFAULT;
	m_radius = 0.3f;
	m_damage = 5;
	init(life, pos, speed, weight, ricochet, color);
}

void PlayerShot_Default::update(float delta)
{
	if (m_lifespan) m_lifespan--;

	m_pos += ((m_speed * ((float)m_lifespan / (float)m_max_lifespan) + 
		(m_speed * 0.2f) * (1.0f - (float)m_lifespan / (float)m_max_lifespan))) * delta;

	Lighting::get()->updateLocalLight(m_light_id, m_pos + Vector3D(0.0, 1.0f, 0.0f), Vector3D(m_color.m_x, m_color.m_y, m_color.m_z) / 2.0f, 1.0f * ((float)m_lifespan / (float)m_max_lifespan));

}



PlayerShot_Machine::PlayerShot_Machine(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	m_type = P_MACHINE;
	m_radius = 0.15f;
	m_damage = 2;
	init(life, pos, speed, weight, ricochet, color);
}

void PlayerShot_Machine::update(float delta)
{
	if (m_lifespan) m_lifespan--;

	m_pos += ((m_speed * ((float)m_lifespan / (float)m_max_lifespan) +
		(m_speed * 0.6f) * (1.0f - (float)m_lifespan / (float)m_max_lifespan))) * delta;

	Lighting::get()->updateLocalLight(m_light_id, m_pos + Vector3D(0.0, 1.0f, 0.0f), Vector3D(m_color.m_x, m_color.m_y, m_color.m_z) / 2.0f, 1.0f * ((float)m_lifespan / (float)m_max_lifespan));

}


PlayerShot_Spread::PlayerShot_Spread(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	m_type = P_SPREAD;
	m_radius = 0.2f;
	m_damage = 4;
	init(life, pos, speed, weight, ricochet, color);
}

void PlayerShot_Spread::update(float delta)
{
	if (m_lifespan) m_lifespan--;

	m_pos += (m_speed) * (1.2f - (float)(m_lifespan + (m_max_lifespan/3.0f)) / (float)m_max_lifespan) * delta;

	Lighting::get()->updateLocalLight(m_light_id, m_pos + Vector3D(0.0, 1.0f, 0.0f), Vector3D(m_color.m_x, m_color.m_y, m_color.m_z) / 2.0f, 1.0f * ((float)m_lifespan / (float)m_max_lifespan));
}


PlayerShot_CurveL::PlayerShot_CurveL(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	m_type = P_SPREAD;
	m_radius = 0.3f;
	m_damage = 15;

	init(life, pos, speed, weight, ricochet, color);

	float orig_speed = atan2(speed.m_z, speed.m_x) / 0.01745f - 60;
	m_speed = Vector3D(cosf(orig_speed * 0.01745f), 0, sinf(orig_speed * 0.01745f)) * speed.length();

	Vector3D dir = m_speed;

	float target_angle = atan2(dir.m_z, dir.m_x) / 0.01745f + 90;
	m_target_speed = Vector3D(cosf(target_angle * 0.01745f), 0, sinf(target_angle * 0.01745f)) * speed.length();


}

void PlayerShot_CurveL::update(float delta)
{
	if (m_lifespan) m_lifespan--;



	m_pos += ((m_speed * ((float)m_lifespan / (float)m_max_lifespan) +
		(m_target_speed) * 2 * (1.0f - (float)m_lifespan / (float)m_max_lifespan))) * delta;

	m_radius = ((0.4f * ((float)m_lifespan / (float)m_max_lifespan) +
		(0.9f) * (1.0f - (float)m_lifespan / (float)m_max_lifespan)));

	Lighting::get()->updateLocalLight(m_light_id, m_pos + Vector3D(0.0, 1.0f, 0.0f), Vector3D(m_color.m_x, m_color.m_y, m_color.m_z) / 2.0f, 1.0f * ((float)m_lifespan / (float)m_max_lifespan));
}



PlayerShot_CurveR::PlayerShot_CurveR(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	m_type = P_SPREAD;
	m_radius = 0.3f;
	m_damage = 15;
	init(life, pos, speed, weight, ricochet, color);

	float orig_speed = atan2(speed.m_z, speed.m_x) / 0.01745f + 60;
	m_speed = Vector3D(cosf(orig_speed * 0.01745f), 0, sinf(orig_speed * 0.01745f)) * speed.length();

	Vector3D dir = m_speed;

	float target_angle = atan2(dir.m_z, dir.m_x) / 0.01745f - 90;
	m_target_speed = Vector3D(cosf(target_angle * 0.01745f), 0, sinf(target_angle * 0.01745f)) * speed.length();


}

void PlayerShot_CurveR::update(float delta)
{
	if (m_lifespan) m_lifespan--;



	m_pos += ((m_speed * ((float)m_lifespan / (float)m_max_lifespan) +
		(m_target_speed) * 2 * (1.0f - (float)m_lifespan / (float)m_max_lifespan))) * delta;

	m_radius = ((0.4f * ((float)m_lifespan / (float)m_max_lifespan) +
		(0.9f) * (1.0f - (float)m_lifespan / (float)m_max_lifespan)));

	Lighting::get()->updateLocalLight(m_light_id, m_pos + Vector3D(0.0, 1.0f, 0.0f), Vector3D(m_color.m_x, m_color.m_y, m_color.m_z) / 2.0f, 1.0f * ((float)m_lifespan / (float)m_max_lifespan));
}


PlayerShot_Bomb::PlayerShot_Bomb(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	m_type = P_BOMB;
	m_radius = 0.5f;
 	m_damage = 0;
	init(life, pos, speed, weight, ricochet, color);
}

PlayerShot_Bomb::~PlayerShot_Bomb()
{
	BulletManager::get()->spawn(P_EXPLOSION, 20, m_pos, Vector3D(0, 0, 0), 1.0f, 99, Vector3D(30.0f, 30.0f, 5.0f));
	if (m_light_id) Lighting::get()->clearLight(m_light_id);
}

void PlayerShot_Bomb::update(float delta)
{
	if (m_lifespan) m_lifespan--;

	m_pos += (m_speed) * (1.2f - (float)(m_lifespan + (m_max_lifespan / 3.0f)) / (float)m_max_lifespan) * delta;

	Lighting::get()->updateLocalLight(m_light_id, m_pos + Vector3D(0.0, 1.0f, 0.0f), Vector3D(m_color.m_x, m_color.m_y, m_color.m_z) / 2.0f, 1.0f * ((float)m_lifespan / (float)m_max_lifespan));
}


PlayerShot_Explosion::PlayerShot_Explosion(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	m_type = P_EXPLOSION;
	m_radius = 0.3f;
	m_damage = 30;
	init(life, pos, speed, weight, ricochet, color);

	m_color.m_w = 0.8f;
}

void PlayerShot_Explosion::update(float delta)
{
	if (m_lifespan) m_lifespan--;

	m_radius = 0.3f + (8.0f * ( 1.0f - ((float)m_lifespan / (float)m_max_lifespan)));

	Lighting::get()->updateLocalLight(m_light_id, m_pos + Vector3D(0.0, 1.0f, 0.0f), Vector3D(m_color.m_x, m_color.m_y, m_color.m_z) / 2.0f,
		(1.0f + (8.0f * (1.0f - ((float)m_lifespan / (float)m_max_lifespan)))) * min((float)m_lifespan / ((float)m_max_lifespan * 0.9f), 1.0f) );

	m_color.m_w = (0.8f * ((float)m_lifespan / (float)m_max_lifespan));
}





EnemyShot_Default::EnemyShot_Default(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color, float radius)
{
	m_type = E_DEFAULT;
	m_radius = radius;
	init(life, pos, speed, weight, ricochet, color);
}

void EnemyShot_Default::update(float delta)
{
	if (m_lifespan) m_lifespan--;

	m_pos += ((m_speed * ((float)m_lifespan / (float)m_max_lifespan) +
		(m_speed * 0.2f) * (1.0f - (float)m_lifespan / (float)m_max_lifespan))) * delta;

	Lighting::get()->updateLocalLight(m_light_id, m_pos + Vector3D(0.0, 1.0f, 0.0f), Vector3D(m_color.m_x, m_color.m_y, m_color.m_z) /2.0f, 1.0f * ((float)m_lifespan / (float)m_max_lifespan));
}





BulletManager* BulletManager::m_bm = nullptr;

BulletManager* BulletManager::get()
{
	if (m_bm == nullptr) m_bm = new BulletManager();

	return m_bm;
}

BulletManager::BulletManager()
{
	loadMeshes();
}

BulletManager::~BulletManager()
{
	if (m_bm) delete m_bm;
}

void BulletManager::update(float delta)
{
	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i]->m_lifespan <= 0) m_bullets.erase(m_bullets.begin() + i);
		else
		{
			m_bullets[i]->update(delta);
		}
	}
}

void BulletManager::render(float delta)
{
	//for (int i = 0; i < m_bullets.size(); i++)
	//{
	//	float angle = atan2(m_bullets[i]->m_speed.m_x, m_bullets[i]->m_speed.m_z);
	//	float d = m_bullets[i]->m_radius * 2;
	//	m_meshes[MeshIndex[m_bullets[i]->getType()]]->renderMesh(delta, Vector3D(d,d,d), m_bullets[i]->m_pos, Vector3D(0, angle, 0), m_bullets[i]->m_color);
	//}
}

void BulletManager::spawn(int type, int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color)
{
	//if we are at max bullet count, ignore the request to spawn a bullet
	if (m_bullets.size() > MAX_BULLETCOUNT)
	{
		return;
	}

	//create a new bullet and push it to the back of the vector
	Bullet* b = nullptr;
	switch (type)
	{
	case P_DEFAULT:
		b = new PlayerShot_Default(life, pos, speed, weight, ricochet, color);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z) / 5.0f, 0.5f);
		break;
	case P_MACHINE:
		b = new PlayerShot_Machine(life, pos, speed, weight, ricochet, color);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z), 0.6f);
		break;
	case P_SPREAD:
		b = new PlayerShot_Spread(life, pos, speed, weight, ricochet, color);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z), 0.1f);
		break;
	case P_CURVE:
		b = new PlayerShot_CurveL(life, pos, speed, weight, ricochet, color);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z), 0.8f);

		m_bullets.push_back(b);

		b = new PlayerShot_CurveR(life, pos, speed, weight, ricochet, color);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z), 0.8f);
		break;
	case P_BOMB:
		b = new PlayerShot_Bomb(life, pos, speed, weight, ricochet, color);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z), 0.2f);
		break;
	case P_EXPLOSION:
		b = new PlayerShot_Explosion(life, pos, speed, weight, ricochet, color);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z), 0.5f);
		break;

	case E_DEFAULT:
		b = new EnemyShot_Default(life, pos, speed, weight, ricochet, color, 0.3f);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z), 1.0f);
		break;
	case E_DEFAULT_GIANT:
		b = new EnemyShot_Default(life, pos, speed, weight, ricochet, color, 0.8f);
		b->m_light_id = Lighting::get()->spawnLight(0, b->m_lifespan, b->m_pos, Vector3D(b->m_color.m_x, b->m_color.m_y, b->m_color.m_z), 1.0f);
		break;
	}
	if (b == nullptr) throw std::exception("Bullet type could not be identified!");



	m_bullets.push_back(b);
}

int BulletManager::checkHit(bool is_player, Vector3D pos, float radius)
{
	if (m_bullets.size() == 0) return false;

	if (is_player) return(checkEnemyShots(pos, radius));
	else return(checkPlayerShots(pos, radius));
}

int BulletManager::checkEnemyShots(Vector3D pos, float radius)
{
	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i]->m_type < P_MAX) continue;

		Vector3D dist = m_bullets[i]->m_pos - pos;
		if (dist.length() < m_bullets[i]->m_radius + radius)
		{
			delete m_bullets[i];
			m_bullets.erase(m_bullets.begin() + i);
			return 1;
		}
	}

	return 0; //no hit
}

int BulletManager::checkPlayerShots(Vector3D pos, float radius)
{
	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i]->m_type > P_MAX) continue;

		Vector3D dist = m_bullets[i]->m_pos - pos;
		if (dist.length() < m_bullets[i]->m_radius + radius)
		{
			int damage = m_bullets[i]->getDamage();

			if (m_bullets[i]->m_ricochet > 1) m_bullets[i]->m_ricochet--;
			else
			{
				delete m_bullets[i];
				m_bullets.erase(m_bullets.begin() + i);
			}
			return damage;
		}
	}

	return 0; //no hit
}

void BulletManager::loadMeshes()
{
	std::vector<std::wstring> files;
	files.push_back(L"..\\Assets\\OBJ\\bullet.fbx");
	files.push_back(L"..\\Assets\\OBJ\\enemybullet.fbx");

	int meshcounter = 0;

	for (int i = 0; i < Bullets::E_MAX; i++)
	{
		//skip the indexes that do not have a mesh
		if (MeshIndex[i] == -1) continue;
		bool is_already_loaded = false;
		for (int j = 0; j < i; j++) if (MeshIndex[j] == MeshIndex[i]) is_already_loaded = true;
		if (is_already_loaded) continue;

		const wchar_t* file = files[meshcounter].c_str();
		meshcounter++;
		SkinnedMeshPtr mesh = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(file, true, nullptr);
		if (mesh == nullptr) throw std::exception("Bullet Mesh could not be loaded!");

		m_meshes.push_back(mesh);
	}
}

