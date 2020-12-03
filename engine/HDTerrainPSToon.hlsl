#include "General.fx"
#include "ZeroToOne.fx"
#include "Sampling.fx"
#include "Lighting.fx"

SamplerState TextureSampler: register(s0);

Texture2D T1: register(t0);
Texture2D N1: register(t1);

Texture2D T2: register(t2);
Texture2D N2: register(t3);

Texture2D T3: register(t4);
Texture2D N3: register(t5);

Texture2D T4: register(t6);
Texture2D N4: register(t7);

Texture2D R1: register(t8);
Texture2D R2: register(t9);
Texture2D R3: register(t10);
Texture2D R4: register(t11);

Texture2D AO1: register(t12);
Texture2D AO2: register(t13);
Texture2D AO3: register(t14);
Texture2D AO4: register(t15);

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

static const float rim_thresh = 0.5;
static const float max_rim = 0.525;

static const float diffuse_thresh = 0.5;
static const float max_diffuse = 0.55;
static const float diffuse_min = 0.3;

static const float specular_thresh = 0.50;
static const float max_specular = 0.70;

static const float AO_range = 120;

static const float atmosphere_height = 300.0;
static const float atmosphere_max_dist = 6200.0 * 2; //more accurately 6189.8

static const float blue_range = 3800.0 * 2;
static const float green_range = 5400.0 * 2;

static const float atmos_fog_range = 2000;

float4 psmain(PS_INPUT input) : SV_TARGET
{
	//--------------------------------------------------------------------------------------//
	//diffuse texture sampling-																//
	//--------------------------------------------------------------------------------------//
	float3 sample_color  = T1.Sample(TextureSampler, input.texcoord);
	float3 sample_color2 = T2.Sample(TextureSampler, input.texcoord);
	float3 sample_color3 = T3.Sample(TextureSampler, input.texcoord);


	//since cliffs are applied to skewed (tall) polygons, they need to use triplanar sampling in order to looknormal.
	float3 triplanar_balance = abs(input.normal);
	triplanar_balance = pow(max(triplanar_balance, 0), 2);
	float2 cliff_coord1 = input.world_pos.zx / 10;  //triplanar_balance.y;
	float2 cliff_coord2 = input.world_pos.xy / 10;  //triplanar_balance.z;
	float2 cliff_coord3 = input.world_pos.zy / 10;  //triplanar_balance.x;
	float3 c_sample1 = T4.Sample(TextureSampler, cliff_coord1);
	float3 c_sample2 = T4.Sample(TextureSampler, cliff_coord2);
	float3 c_sample3 = T4.Sample(TextureSampler, cliff_coord3);

	float3 sample_color4 = c_sample1 * triplanar_balance.y + c_sample2 * triplanar_balance.z + c_sample3 * triplanar_balance.x;


	float3 color = getTextureSplat(sample_color, sample_color2, sample_color3, input.texbias.rgb);

	//if necessary, mix the cliff texture
	float3 final_sample = color * (input.cliff_face < 0.7) + sample_color4 * (input.cliff_face >= 0.7);

	//-----------------------------------------------------------//
	//Normal Calculation
	//-----------------------------------------------------------//
	float3 sample_normal = N1.Sample(TextureSampler, input.texcoord);
	float3 sample_normal2 = N2.Sample(TextureSampler, input.texcoord);
	float3 sample_normal3 = N3.Sample(TextureSampler, input.texcoord);

	//cliff face sample
	float3 n_sample1 = N4.Sample(TextureSampler, cliff_coord1);
	float3 n_sample2 = N4.Sample(TextureSampler, cliff_coord2);
	float3 n_sample3 = N4.Sample(TextureSampler, cliff_coord3);

	float3 sample_normal4 = n_sample1 * triplanar_balance.y + n_sample2 * triplanar_balance.z + n_sample3 * triplanar_balance.x;

	float3 normal = getTextureSplat(sample_normal, sample_normal2, sample_normal3, input.texbias.rgb);

	//if the texbias for this face is over the cliff threshhold, mix the cliff normal
	normal = normal * (input.cliff_face < 0.7) + sample_normal4 * (input.cliff_face >= 0.7);

	//build the domain, then adjust the normal map sample to the normal of the domain
	float3x3 inverseMatrix = buildInverseMatrix(input.normal, input.binormal, input.tangent);
	float3 inverseToCamera = normalize(mul(inverseMatrix, input.direction_to_camera));
	normal = normalize(mul(normal, inverseMatrix));
	normal = normal * 2 - 1;
	float temp = normal.z;
	normal.z = normal.y;
	normal.y = temp;

	float dist = length(input.world_pos - m_camera_position.xyz);

	//--------------------------------------------------------------------------------------//
	//AO texture sampling	-																//
	//--------------------------------------------------------------------------------------//
	float sample_ao = AO1.Sample(TextureSampler, input.texcoord);
	float sample_ao2 = AO2.Sample(TextureSampler, input.texcoord);
	float sample_ao3 = AO3.Sample(TextureSampler, input.texcoord);
	float sample_ao4 = AO4.Sample(TextureSampler, input.cliff_texcoord);

	float ao = getTextureSplat(sample_ao, sample_ao2, sample_ao3, input.texbias.rgb);

	//if necessary, mix the cliff texture
	ao = ao * (1.0 - input.cliff_face) + sample_ao4 * input.cliff_face;
	ao = ao * (1 - min(dist / AO_range, 1)) + min(dist / AO_range, 1);

	//--------------------------------------------------------------------------------------//
	//Roughness texture sampling	-														//
	//--------------------------------------------------------------------------------------//
	float sample_rough =  R1.Sample(TextureSampler, input.texcoord);
	float sample_rough2 = R2.Sample(TextureSampler, input.texcoord);
	float sample_rough3 = R3.Sample(TextureSampler, input.texcoord);
	float sample_rough4 = R4.Sample(TextureSampler, input.cliff_texcoord);

	float roughness = getTextureSplat(sample_rough, sample_rough2, sample_rough3, input.texbias.rgb);

	//if necessary, mix the cliff texture
	roughness = roughness * (1.0 - input.cliff_face) + sample_rough4 * input.cliff_face;
	roughness = roughness * (1 - min(dist / AO_range, 1)) + min(dist / AO_range, 1);

	//--------------------------------------------------------------------------------------//
	//lighting calculations -																//
	//--------------------------------------------------------------------------------------//

	float3 direction_to_camera = normalize(input.world_pos.xyz - m_camera_position.xyz);
	float3 light_direction = normalize(m_global_light_dir.xyz);

	//diffuse lighting
	//float faceDot = max(0.0, dot(input.normal, light_direction));
	//float diffuseAmount = min(max((diffuse_thresh - faceDot) / (diffuse_thresh - max_diffuse), 0), 1)
	//	* (faceDot > diffuse_thresh);
	//float3 diffuseReflection = m_global_light_strength * m_global_light_color.xyz * diffuseAmount;
	
	float3 diffuseReflection = gradientOneCellDiffuse(input.normal, light_direction, m_global_light_color, 0.4f);
	
	//specular
	float specDot = pow(max(0.0, dot(reflect(-light_direction, input.normal), -direction_to_camera)), 10);
	float specAmount = min(max((specular_thresh - specDot) / (specular_thresh - max_specular), 0), 1) * (specDot > specular_thresh);
	float3 specularReflection = m_specularColor.rgb * specAmount;// *roughness;

	//rim lighting
	float rim = pow(1 - dot(-direction_to_camera, input.normal), m_rimPower) * max(dot(light_direction, input.normal), 0);
	float rimlight_amount = min(max((rim_thresh - rim) / (rim_thresh - max_rim), 0), 1) * (rim > rim_thresh);
	float3 rimLighting = m_rimColor.w * m_global_light_strength * m_global_light_color.rgb * m_rimColor.xyz * rimlight_amount;

	//ambient amount
	float3 ambient = m_ambient_light_color * max(0.0, dot(input.normal, -light_direction)) + m_ambient_light_color;

	float3 lightFinal = (rimLighting + diffuseReflection + specularReflection + ambient);// *ao;


	//atmospheric fog
	float3 sun_ref_pos = light_direction * float3(atmosphere_max_dist / 2, atmosphere_height, atmosphere_max_dist / 2);
	//float3 sky_ref_pos = n * float3(atmosphere_height, atmosphere_height, atmosphere_height);

	float sun_dist = length(input.world_pos - sun_ref_pos);
	float3 atmos_fog;
	atmos_fog.r = 1;
	atmos_fog.g = max((green_range - sun_dist) / (green_range / 2), 0);
	atmos_fog.b = max((blue_range - sun_dist) / (blue_range / 2.5), 0);
	atmos_fog = normalize(atmos_fog);

	float3 final = (lightFinal * final_sample) *(1 - min(dist / atmos_fog_range, 1)) + atmos_fog * min(dist / atmos_fog_range, 1);



	return float4(final, 1);
}