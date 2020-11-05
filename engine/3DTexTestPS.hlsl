Texture3D Texture: register(t0);
SamplerState TextureSampler: register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
};

//noise settings
cbuffer constant: register(b5)
{
	float4 m_noise_type;
	float4 m_rgba;

	float  m_vor_octaves;
	float  m_vor_frequency;
	float  m_vor_gain;
	float  m_vor_lacunarity;
	float  m_vor_amplitude;
	float  m_vor_cell_size;

	float  m_per_octaves;
	float  m_per_frequency;
	float  m_per_gain;
	float  m_per_lacunarity;
	float  m_per_amplitude;
	float  m_per_cell_size;

}



float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 location = (input.world_pos) / 1.0;
	//location.z = 0;


	float3 choice = frac(location);
	float c1 = Texture.Sample(TextureSampler, choice).x;
	float c2 = Texture.Sample(TextureSampler, choice).y;
	float c3 = Texture.Sample(TextureSampler, choice).z;
	float c4 = Texture.Sample(TextureSampler, choice).w;

	float3 color = c1 * m_rgba.r + c2 * m_rgba.g + c3 * m_rgba.b + c4 * m_rgba.a;

	//float3 color = c3;

	//bool inside = true;
	//inside = inside * (location.x >= -1);
	//inside = inside * (location.x <= 1);
	//inside = inside * (location.y >= -1);
	//inside = inside * (location.y <= 1);
	//inside = inside * (location.z >= -1);
	//inside = inside * (location.z <= 1);

	//if (!inside) color = 1;

	//color = color.xyz * (input.world_pos.x < 10 && input.world_pos.y < 10 && input.world_pos.z < 10);
	return float4(color, 1);
}