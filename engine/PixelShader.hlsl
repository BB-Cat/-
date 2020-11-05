Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
	float4 bonecolor: WEIGHTS;
};

//world and camera settings
cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
};

//mesh lighting characteristics 
cbuffer constant: register(b1)
{
	float	Ns; //specular power
	float	Ni; //optical density
	float	d;	//transparency 
	float	Tr;   //transparency 
	float	Tf; //transmission filter
	float3	Ka; //ambient color
	float3	Kd; //diffuse color
	float3	Ks; //specular color
	float	Ke; //emissive color
};

//scene lighting settings
cbuffer constant: register(b2)
{
	float m_ambient_strength;
	float3 m_ambient_light;
	float3 m_light_position;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 sample_color = Texture.Sample(TextureSampler, input.texcoord);

	float3 ambient_light = m_ambient_light * m_ambient_strength;

	float3 applied_light = ambient_light * input.bonecolor;

	float3 vector_to_light = normalize(m_light_position - input.world_pos);

	float3 diffuse_light_intensity = max(dot(vector_to_light, input.normal), 0);

	float distance_to_light = distance(m_light_position, input.world_pos);

	float attenuation_factor = 1 / (1.0f + 0.1f * distance_to_light + 0.1f * pow(distance_to_light, 2));

	diffuse_light_intensity *= attenuation_factor;

	float light_strength = 10.0f;
	float3 light_color = float3(1.0f, 1.0f, 1.0f);

	float3 diffuse_light = diffuse_light_intensity * light_strength;

	applied_light += diffuse_light;

	float3 final_color = applied_light * sample_color;


	return float4(final_color, 1.0f);
}



