SamplerState TextureSampler: register(s0);

Texture2D T1: register(t0);
Texture2D N1: register(t1);

Texture2D T2: register(t2);
Texture2D N2: register(t3);

Texture2D T3: register(t4);
Texture2D N3: register(t5);

Texture2D T4: register(t6);
Texture2D N4: register(t7);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD1;
	float3 normal: NORMAL0;
	float3 binormal: NORMAL1;
	float3 tangent: NORMAL2;
	float4 texbias: TEXCOORD2;
	float4 world_pos: TEXCOORD3;
	float cliff_amount : NORMAL3;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_camera_position;
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
	//texture sample
	float3 sample_color = T1.Sample(TextureSampler, input.texcoord);
	float3 sample_color2 = T2.Sample(TextureSampler, input.texcoord);
	float3 sample_color3 = T3.Sample(TextureSampler, input.texcoord);
	//cliff face sample
	float3 sample_color4 = T4.Sample(TextureSampler, input.texcoord * 2);

	float3 final_sample = sample_color * input.texbias.r + sample_color2 * input.texbias.g + sample_color3 * input.texbias.b;
	//if necessary, mix the cliff texture
	final_sample = final_sample * (1.0 - input.cliff_amount) + sample_color4 * input.cliff_amount;



	//--------------------------------------------------------------------------------------//
	//lighting calculations -																//
	//--------------------------------------------------------------------------------------//
	float3 direction_to_camera = normalize(input.world_pos.xyz - m_camera_position.xyz);

	//diffuse lighting
	float3 light_direction = normalize(m_global_light_dir.xyz);
	float3 diffuseReflection = m_global_light_strength * m_global_light_color.xyz * max(0.0, dot(input.normal, light_direction));

	////ambient lighting
	//float3 ambientLightDir = light_direction * -1;
	//float ambientAmount = max(0.0, dot(input.normal, ambientLightDir)) / 2.0 + 0.5;
	//float3 ambientReflection = 0.5 * m_ambient_light_color.rgb * ambientAmount;

	//specular
	float3 specularReflection = m_specularColor.rgb * max(0.0, dot(input.normal, light_direction))
		* pow(max(0.0, dot(reflect(-light_direction, input.normal), -direction_to_camera)), m_shininess);

	//rim lighting
	float rim = 1 - saturate(dot(-direction_to_camera, input.normal));
	float rimlight_amount = max(dot(light_direction, input.normal), 0);
	float3 rimLighting = m_rimColor.w * m_global_light_strength * m_global_light_color.rgb * m_rimColor.xyz * rimlight_amount * pow(rim, m_rimPower);

	float3 lightFinal = rimLighting + diffuseReflection + specularReflection + m_ambient_light_color;


	return float4(final_sample * lightFinal, 1);
}