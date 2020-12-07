#include "Lighting.fx"

struct VS_INPUT
{
	float4 position: POSITION0;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float4 boneweights: WEIGHTS;
	uint4  boneindices: BONES;
};

struct VS_OUTPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD1;
	float4 texbias: TEXCOORD2;
	float3 light: NORMAL0;
	float cliff_amount : NORMAL1;
	float3 fog_color: TEXCOORD3;
	float fog_amount : NORMAL2;
	float ambient_amount : NORMAL3;

};

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_camera_position;
};

#define MAX_BONES 32 
cbuffer constant: register(b1)
{
	row_major float4x4 m_global;
	row_major float4x4 m_bonetransforms[MAX_BONES];
	row_major float4x4 m_coordinate_system;
};

//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}

#define NUM_SHADOW_MAPS (3)
cbuffer constant: register(b3)
{
	float4 m_active_proj;
	row_major float4x4 m_light_world;
	row_major float4x4 m_light_view;
	row_major float4x4 m_light_proj[NUM_SHADOW_MAPS];
};

//terrain colors
cbuffer constant: register(b4)
{
	float4 color1; //color averages for each type of terrain
	float4 color2; //color averages for each type of terrain
	float4 color3; //color averages for each type of terrain
};

static const float cliff_threshhold = 0.6f;
static const float max_cliff = 0.5f;

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

VS_OUTPUT vsmain(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = input.position;

	//--------------------------------------------------------------------------------------//
	//			screen space transformations												//
	//--------------------------------------------------------------------------------------//
	//WORLD SPACE
	output.position = mul(mul(output.position, m_world), m_global);
	float3 world_pos = output.position;
	//texture coordinates
	output.texcoord = float2((output.position.x), (output.position.z)) / 30;
	//VIEW SPACE
	output.position = mul(output.position, m_view);
	//SCREEN SPACE
	output.position = mul(output.position, m_proj);


	//--------------------------------------------------------------------------------------//
	//lighting calculations -																//
	//--------------------------------------------------------------------------------------//

	float3 direction_to_camera = normalize(world_pos - m_camera_position.xyz);
	float3 light_direction = normalize(m_global_light_dir.xyz);

	//diffuse lighting
	//float faceDot = max(0.0, dot(input.normal, light_direction));
	//float diffuseAmount = min(max((diffuse_thresh - faceDot) / (diffuse_thresh - max_diffuse), 0), 1)
	//	* (faceDot > diffuse_thresh);
	//float3 diffuseReflection = m_global_light_strength * m_global_light_color.xyz * diffuseAmount;
	float3 diffuseReflection = gradientOneCellDiffuse(input.normal, light_direction, m_global_light_color, 0.6f);



	//rim lighting
	//float rim = pow(1 - dot(-direction_to_camera, input.normal), 2) * max(dot(light_direction, input.normal), 0);
	//float rimlight_amount = min(max((rim_thresh - rim) / (rim_thresh - max_rim), 0), 1) * (rim > rim_thresh);
	//float3 rimLighting = 1 * m_global_light_strength * m_global_light_color.rgb * float3(0.7, 0.4, 0.4)  * rimlight_amount;
	float3 rimLighting = rim(input.normal, light_direction, float3(1,1,1), direction_to_camera, 2);

	//ambient amount
	//float3 ambient = m_ambient_light_color * max(0.0, dot(input.normal, -light_direction)) + m_ambient_light_color;
	output.ambient_amount = max(0.0, dot(input.normal, -light_direction));

	output.light = diffuseReflection;

	//read the texture type from the input's boneweights
	float total_tex = input.boneweights.r + input.boneweights.g + input.boneweights.b;
	float r = input.boneweights.r / total_tex;
	float g = input.boneweights.g / total_tex;
	float b = input.boneweights.b / total_tex;
	output.texbias = float4(r, g, b, input.boneweights.w);

	//if the texbias for this face is over the cliff threshhold, mix the cliff height
	output.cliff_amount = min(max((cliff_threshhold - input.boneweights.w) / (cliff_threshhold - max_cliff), 0), 1);


	//atmospheric fog
	float3 sun_ref_pos = light_direction * float3(atmosphere_max_dist / 2, atmosphere_height, atmosphere_max_dist / 2);
	//float3 sky_ref_pos = n * float3(atmosphere_height, atmosphere_height, atmosphere_height);

	float sun_dist = length(world_pos - sun_ref_pos);
	float3 atmos_fog;
	atmos_fog.r = 1;
	atmos_fog.g = max((green_range - sun_dist) / (green_range / 2), 0);
	atmos_fog.b = max((blue_range - sun_dist) / (blue_range / 2.5), 0);
	output.fog_color = normalize(atmos_fog);

	float dist = length(world_pos - m_camera_position.xyz);
	output.fog_amount = min(dist / atmos_fog_range, 1);

	return output;

}
