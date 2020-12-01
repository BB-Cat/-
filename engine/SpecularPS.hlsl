#include "Lighting.fx"

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float4 world_pos : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 direction_to_camera : NORMAL1;
	float4 light_ambient: TEXCOORD1;
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
	float atten = m_global_light_strength;
	float3 light_dir = normalize(m_global_light_dir.xyz);

	//diffuse
	float3 diffuse_reflection = diffuse(input.normal, light_dir, m_global_light_color.rgb);

	//specular
	float3 specular_reflection = spec(input.normal, light_dir, m_specularColor.rgb, input.direction_to_camera, m_shininess);

	//final
	float3 lightFinal = (diffuse_reflection + specular_reflection) * atten + input.light_ambient;
	return float4(m_diffuseColor.rgb * lightFinal, m_d);

}