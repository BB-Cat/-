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

//not currently being used as a proper global light, its instead being used as a wierd ambient light source. needs fixing.
//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_global_light_position;
}

#define NUM_LIGHTS (20)

cbuffer LightPositionBuffer: register(b3)
{
	float4 m_light_pos[NUM_LIGHTS];
	float4 m_light_color[NUM_LIGHTS];
	float4 m_light_strength[NUM_LIGHTS];
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 sample_color = Kd;

	float3 diffuse_light = 0.0f;

	float rimlight = 0.0f;

	float3 ambient_light = float3(0.2f, 0.2f, 0.2f);
	float3 applied_light = ambient_light;

	if (m_global_light_strength > 0)
	{
		rimlight = 1.0f - ((max(dot(m_global_light_dir, input.normal), 0)));
		//diffuse_light += diffuse_light_intensity * m_global_light_strength * m_global_light_color.xyz;
		diffuse_light += m_global_light_strength * m_global_light_color.xyz;

	}

	//calculate the effect each light has on the pixel
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		//if the light source is active, calculate lighting
		if (m_light_strength[0].x > 0)
		{
				float3 vector_to_light = normalize(m_light_pos[i].xyz - input.world_pos);
			
				float3 diffuse_light_intensity = max(dot(vector_to_light, input.normal), 0);
				//float3 diffuse_light_intensity = max(dot(vector_to_light, float3(0, 1, 0)), 0);
				float distance_to_light = distance(m_light_pos[i].xyz, input.world_pos);
				float attenuation_factor = 1 / (1.0f + 0.01f * distance_to_light + 0.2f * pow(distance_to_light, 2));

				diffuse_light_intensity *= attenuation_factor;
				diffuse_light += diffuse_light_intensity * m_light_strength[i].x * m_light_color[i].xyz;
		}

	}

	applied_light += diffuse_light;

	float3 final_color = applied_light * sample_color;/* +(m_global_light_color * m_global_light_strength);*/


	return float4(final_color, d);
}


float4 pssprite(PS_INPUT input) : SV_TARGET
{
	float4 sample_color = Texture.Sample(TextureSampler, input.texcoord);

	return float4(sample_color);
	//return sample_color;
}



float4 psterrain(PS_INPUT input) : SV_TARGET
{
	float3 sample_color = input.bonecolor;

	float3 diffuse_light = 0.0f;



	float3 ambient_light = float3(0.2f, 0.2f, 0.2f);
	float3 applied_light = ambient_light;

	if (m_global_light_strength > 0)
	{
		//float3 diffuse_light_intensity = max(dot(m_global_light_dir, input.normal), 0);
		//diffuse_light += diffuse_light_intensity * m_global_light_strength * m_global_light_color.xyz;
		diffuse_light += m_global_light_strength * m_global_light_color.xyz;

	}

	//calculate the effect each light has on the pixel
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		//if the light source is active, calculate lighting
		if (m_light_strength[0].x > 0)
		{
			float3 vector_to_light = normalize(m_light_pos[i].xyz - input.world_pos);
			float3 diffuse_light_intensity = max(dot(vector_to_light, input.normal), 0);
			float distance_to_light = distance(m_light_pos[i].xyz, input.world_pos);
			float attenuation_factor = 1 / (1.0f + 0.01f * distance_to_light + 0.2f * pow(distance_to_light, 2));

			diffuse_light_intensity *= attenuation_factor;
			diffuse_light += diffuse_light_intensity * m_light_strength[i].x * m_light_color[i].xyz;
		}

	}

	applied_light += diffuse_light;

	float3 final_color =applied_light * sample_color;
	//float3 final_color = sample_color;


	return float4(final_color, 1.0f);
}