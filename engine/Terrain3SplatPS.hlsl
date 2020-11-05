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
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 binormal : NORMAL1;
	float3 tangent : NORMAL2;
	float4 texbias : WEIGHTS;
	float3 direction_to_camera : NORMAL3;
	float4 lightcolor: TEXCOORD2;
	float cliff_face : WEIGHTS1;
	float2 cliff_texcoord : TEXCOORD3;
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

static const float cliff_threshhold = 0.6f;
static const float max_cliff = 0.5f;

float4 psmain(PS_INPUT input) : SV_TARGET
{
	//float3 normal = N1.Sample(TextureSampler, input.texcoord).xyz;

	float3 sample_normal =  N1.Sample(TextureSampler, input.texcoord);
	float3 sample_normal2 = N2.Sample(TextureSampler, input.texcoord);
	float3 sample_normal3 = N3.Sample(TextureSampler, input.texcoord);
	//cliff face sample
	float3 sample_normal4 = N4.Sample(TextureSampler, input.texcoord * 2);

	float3 normal =
		sample_normal * (input.texbias.r > input.texbias.g && input.texbias.r > input.texbias.b) +
		sample_normal2 * (input.texbias.g > input.texbias.r && input.texbias.g > input.texbias.b) +
		sample_normal3 * (input.texbias.b >= input.texbias.r && input.texbias.b >= input.texbias.g);

	//if the texbias for this face is over the cliff threshhold, mix the cliff normal
	normal = normal * (1.0 - input.cliff_face) + sample_normal4 * input.cliff_face;

	//adjust the normal map sample to the normal of the domain
	float3 vx = normalize(input.tangent);
	float3 vy = normalize(input.binormal);
	float3 vz = normalize(input.normal);

	float3x3 inverseMatrix = { vx,vy,vz };
	float3 inverseToCamera = normalize(mul(inverseMatrix, input.direction_to_camera));

	float3x3 mat = { vx,vy,vz };
	normal = normalize(mul(normal, mat));
	normal = normal * 2 - 1;
	float temp = normal.z;
	normal.z = normal.y;
	normal.y = temp;

	//direction to camera for specular and rim lighting
	float3 direction_to_camera = normalize(input.world_pos.xyz - m_camera_position.xyz);

	//--------------------------------------------------------------------------------------//
	//lighting calculations -																//
	//--------------------------------------------------------------------------------------//
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
	

	//diffuse texture sampling
	float3 sample_color = T1.Sample(TextureSampler, input.texcoord);
	float3 sample_color2 = T2.Sample(TextureSampler, input.texcoord);
	float3 sample_color3 = T3.Sample(TextureSampler, input.texcoord);
	float3 sample_color4 = T4.Sample(TextureSampler, input.input.texcoord * 2);

	float r = input.texbias.r / (input.texbias.r + input.texbias.g + input.texbias.b);
	float g = input.texbias.g / (input.texbias.r + input.texbias.g + input.texbias.b);
	float b = input.texbias.b / (input.texbias.r + input.texbias.g + input.texbias.b);
	
	float3 final_sample = sample_color * r + sample_color2 * g + sample_color3 * b;

	//if necessary, mix the cliff texture
	final_sample = final_sample * (1.0 - input.cliff_face) + sample_color4 * input.cliff_face;



	return float4(lightFinal * final_sample, 1);
	//return float4(1, 1, 1, 1);
}