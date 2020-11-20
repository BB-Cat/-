#include "noise.fx"

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
};



//static const float cell_size = 0.25;
static const float period = 9;

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 value = input.world_pos.xyz / m_vor_cell_size;
	
	float noise = (1.0 - brownianTiledVoronoi(value, (int)m_vor_octaves, (int)m_vor_frequency, m_vor_gain, m_vor_lacunarity, m_vor_amplitude).x) * m_noise_type.w;
	float noise2 = (brownianTiledPerlin(value, (int)m_per_octaves, (int)m_per_frequency, m_per_gain, m_per_lacunarity, m_per_amplitude).x + 0.5) * m_noise_type.z;

	//float3 final = (1.0 - brownianTiledVoronoi(value, (int)m_octaves, (int)m_frequency, m_gain, m_lacunarity, m_amplitude).x);

	float3 final = noise + noise2;
	return float4(final, 1);
}