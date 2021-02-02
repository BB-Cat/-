#include "Lighting.fx"

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 world_pos : TEXCOORD0;
};

//mesh lighting characteristics 
cbuffer constant: register(b1)
{
	float		m_shininess; //specular power
	float		m_rimPower; //rimlight power
	float		m_d;	//transparency 
	float		m_metallicAmount; //amount of metallicism
	float4		m_ambientColor; //ambient color
	float4		m_diffuseColor; //diffuse color
	float4		m_specularColor; //specular color
	float4		m_emitColor; //emissive color
	float4		m_rimColor; //rimlight color
};

//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	//offset the world pos by half of the stroke width so that the center 
	//of the stroke is perfectly aligned with the unit of measurement
	float absx = abs(input.world_pos.x);
	float absz = abs(input.world_pos.z);

	float x = (absx % 1 < 0.02 || absx % 1 > 9.98);						   
	float z = (absz % 1 < 0.02 || absz % 1 > 9.98);

	return 1 * (x || z);
}