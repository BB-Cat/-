#pragma once
#include "Prerequisites.h"
#include "ConstantBufferFormats.h"
#include "Vector3D.h"
#include <vector>

class Lighting;

class Light
{
public:
	Light(int id, int lifespan, Vector3D pos, Vector3D color, float strength) :
		m_id(id), m_lifespan(lifespan), m_max_lifespan(lifespan), m_pos(pos), m_color(color), m_strength(strength), m_orig_strength(strength), m_is_erase(false) {}

	~Light() {}

	//virtual void update() = 0;

protected:
	int m_id;
	int m_lifespan;
	int m_max_lifespan;
protected:
	Vector3D m_pos;
	Vector3D m_color;
	float m_strength;
	float m_orig_strength;
	bool m_is_erase;

private:
	friend class Lighting;
};

//class Light_Fade : public Light
//{
//	// Light ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
//	//virtual void update() override;
//};


class Lighting
{
public:
	static Lighting* Lighting::get()
	{
		if (l == nullptr) l = new Lighting();
		return l;
	}

	~Lighting();
public:
	//update the constant lighting buffer with data from m_lights vector
	void setLights();
	//add a new light to the m_lights vector.  returns 0 if the number of lights is at max.
	int  spawnLight(int type, int lifespan, Vector3D pos, Vector3D color, float strength);
	//deletes the light with [id] in the m_lights vector
	void clearLight(int id);
	//updates the light with [id] in the m_lights vector.  returns false if no light is found with the id number.
	bool updateLocalLight(const int& id, const Vector3D& pos, const Vector3D& color, const float& strength);
	//update the scene lighting with new information
	void updateSceneLight(const Vector3D& dir, const Vector3D& color, const float& strength, const Vector3D& ambient_color);
	
public:
	//control interface function to change lighting properties in game
	void sceneLightingInput(float delta);

private:
	static Lighting* l;
	static int m_next_light_id;
	Lighting();

	std::vector<Light> m_lights;
	cb_scene_lighting m_sl;
};
