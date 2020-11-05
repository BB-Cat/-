#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include <vector>
#include "SkinnedMesh.h"

class BulletManager; //declaration of bulletmanager class in order to make it a friend of bullet class

class Bullet
{
public:
	Bullet() {}
	virtual ~Bullet();
public:
	void init(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);
	virtual void update(float delta) = 0;
	int getType() { return m_type; }
	int getDamage() { return m_damage; }

protected:
	int m_lifespan;		//duration of time that the bullet lasts
	int m_max_lifespan; //record of the original lifespan value to make decaying / growing calculations on effects as necessary
	Vector3D m_speed;	//speed that the bullet travels
	Vector3D m_pos;		//position of the bullet
	float m_radius;
	float m_weight;		//how much the bullet pushes things when it collides with them
	int m_ricochet;    //determines the amount of times bullet bounces off of walls
	int m_damage;
	Vector4D m_color;  //color of the mesh

protected:
	int m_type;		   //int which shows the manager what type of bullet it is
	int m_light_id;    //id number for the lightsource being updated by the bullet
protected:
	friend class BulletManager;
};


/////SHOT TYPES //////
class PlayerShot_Default : public Bullet
{
public:
	PlayerShot_Default(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);

	// Bullet ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
};

class PlayerShot_Machine : public Bullet
{
public:
	PlayerShot_Machine(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);

	// Bullet ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
};

class PlayerShot_Spread : public Bullet
{
public:
	PlayerShot_Spread(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);

	// Bullet ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
};

class PlayerShot_CurveL : public Bullet
{
private:
	Vector3D m_target_speed;
public:
	PlayerShot_CurveL(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);

	// Bullet ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
};

class PlayerShot_CurveR : public Bullet
{
private:
	Vector3D m_target_speed;
public:
	PlayerShot_CurveR(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);

	// Bullet ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
};

class PlayerShot_Bomb : public Bullet
{

public:
	PlayerShot_Bomb(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);
	virtual ~PlayerShot_Bomb() override;

	// Bullet ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
};

class PlayerShot_Explosion : public Bullet
{

public:
	PlayerShot_Explosion(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);

	// Bullet ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
};

class EnemyShot_Default : public Bullet
{
public:
	EnemyShot_Default(int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color, float radius);

	// Bullet ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
	virtual void update(float delta) override;
};
//////////////////////

//enumeration for bullet meshes
enum Bullets
{
	P_DEFAULT,
	P_MACHINE,
	P_SPREAD,
	P_CURVE,
	P_BOMB,
	P_EXPLOSION,
	P_MAX, //tells the manager where the friendly bullet types end and the enemy bullet types begin
	E_DEFAULT,
	E_DEFAULT_GIANT,
	E_MAX
};


#define MAX_BULLETCOUNT (500)

class BulletManager
{
public:
	static BulletManager* get();
	~BulletManager();
public:
	void update(float delta);
	void render(float delta);
	void spawn(int type, int life, Vector3D pos, Vector3D speed, float weight, int ricochet, Vector3D color);
	void initBullets() { if(m_bullets.size() != 0) m_bullets.clear(); }
public:
	//returns 0 if no hit, or returns the amount of damage
	int checkHit(bool is_player, Vector3D pos, float radius);
private:
	int checkEnemyShots(Vector3D pos, float radius);
	int checkPlayerShots(Vector3D pos, float radius);
private:
	void loadMeshes();
private:
	BulletManager();
	static BulletManager* m_bm;
	std::vector<Bullet*> m_bullets;
	std::vector<SkinnedMeshPtr> m_meshes;
};