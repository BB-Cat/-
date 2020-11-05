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

static const float rim_thresh = 0.5;
static const float max_rim = 0.525;

static const float diffuse_thresh = 0.5;
static const float max_diffuse = 0.55;
static const float diffuse_min = 0.3;

static const float specular_thresh = 0.50;
static const float max_specular = 0.70;

static const float atmosphere_height = 300.0;
static const float atmosphere_max_dist = 6200.0 * 2; //more accurately 6189.8

static const float blue_range = 3800.0 * 2;
static const float green_range = 5400.0 * 2;

static const float atmos_fog_range = 2000;

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
	float3 light_direction = normalize(m_global_light_dir.xyz);

	//diffuse lighting
	float faceDot = max(0.0, dot(input.normal, light_direction));
	float diffuseAmount = min(max((diffuse_thresh - faceDot) / (diffuse_thresh - max_diffuse), 0), 1)
		* (faceDot > diffuse_thresh);
	//diffuseAmount = diffuseAmount * 0.5 + 0.5;
	float3 diffuseReflection = m_global_light_strength * m_global_light_color.xyz * diffuseAmount;

	//specular
	float specDot = pow(max(0.0, dot(reflect(-light_direction, input.normal), -direction_to_camera)), m_shininess);
	float specAmount = min(max((specular_thresh - specDot) / (specular_thresh - max_specular), 0), 1) * (specDot > specular_thresh);
	float3 specularReflection = m_specularColor.rgb * specAmount;

	//rim lighting
	float rim = pow(1 - dot(-direction_to_camera, input.normal), m_rimPower) * max(dot(light_direction, input.normal), 0);
	float rimlight_amount = min(max((rim_thresh - rim) / (rim_thresh - max_rim), 0), 1) * (rim > rim_thresh);
	float3 rimLighting = m_rimColor.w * m_global_light_strength * m_global_light_color.rgb * m_rimColor.xyz * rimlight_amount;

	//ambient amount
	float3 ambient = m_ambient_light_color * max(0.0, dot(input.normal, -light_direction)) + m_ambient_light_color;

	float3 lightFinal = rimLighting + diffuseReflection + specularReflection + ambient;

	//atmospheric fog
	float3 sun_ref_pos = light_direction * float3(atmosphere_max_dist / 2, atmosphere_height, atmosphere_max_dist / 2);
	//float3 sky_ref_pos = n * float3(atmosphere_height, atmosphere_height, atmosphere_height);

	float sun_dist = length(input.world_pos - sun_ref_pos);
	float3 atmos_fog;
	atmos_fog.r = 1;
	atmos_fog.g = max((green_range - sun_dist) / (green_range / 2), 0);
	atmos_fog.b = max((blue_range - sun_dist) / (blue_range / 2.5), 0);
	atmos_fog = normalize(atmos_fog);

	float dist = length(input.world_pos - m_camera_position.xyz);
	float3 final = (lightFinal * final_sample) * (1 - min(dist / atmos_fog_range, 1)) + atmos_fog * min(dist / atmos_fog_range, 1);


	return float4(final, 1);

	//return float4(final_sample * lightFinal, 1);
}