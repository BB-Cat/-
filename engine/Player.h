#pragma once
#include "Vector2D.h"
#include "high_resolution_timer.h"
#include "Actor.h"

#define PSPEED_MAX      (10.0f)
#define PACCEL			(0.2f)		//player deceleration
#define PROT_SPEED_MAX	(5.0f)		//player rotation speed
#define PROT_ACCEL		(1.2f)		//player rotation acceleration
#define PJUMP_FORCE		(0.1f)		//player starting jump speed
#define PJUMP_DECAY		(0.004f)	//player jump speed decay rate

//bullet speeds
#define PSHOT_SPEED		(30.0f)		//speed of players default bullets
#define PMACHINE_SPEED		(65.0f)		//speed of players machine bullets
#define PSPREAD_SPEED		(45.0f)		//speed of players spread bullets
#define PCURVE_SPEED		(5.0f)		//speed of players spread bullets
#define PBOMB_SPEED			(50.0f)		//speed of players bomb bullets


// shot colors for player
#define PDEFAULT_COLOR  (Vector3D(5.0f, 5.0f, 10.0f))
#define PMACHINE_COLOR  (Vector3D(4.0f, 8.0f, 4.0f))
#define PSPREAD_COLOR  (Vector3D(7.0f, 7.0f, 2.0f))
#define PCURVE_COLOR  (Vector3D(7.0f, 5.0f, 7.0f))
#define PBOMB_COLOR  (Vector3D(13.0f, 3.0f, 3.0f))

#define PLAYER_RADIUS (0.35f)
#define PLAYER_RESPAWN (2.0f)
#define PLAYER_INVINCIBLE (3.0f)

class Player : public Actor
{
public:
	Player();
	~Player();
public:
	void update(float delta, Terrain* terrain);
	void render(float delta);

private: //rendering members
	SkinnedMeshPtr m_mesh;

private:
	friend class ActorManager;
};

