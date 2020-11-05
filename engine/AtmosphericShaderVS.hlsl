//struct VS_INPUT
//{
//	float4 position: POSITION0;
//	float2 texcoord: TEXCOORD0;
//	float3 normal: NORMAL0;
//	float4 boneweights: WEIGHTS;
//	uint4  boneindices: BONES;
//};
//
//struct VS_OUTPUT
//{
//	float4 position: SV_POSITION;
//	float3 color: COLOR0;
//};
//
//static const float atmosphere_height = 300.0;
//static const float atmosphere_max_dist = 6200.0 * 2; //more accurately 6189.8
//
//static const float blue_range = 4800.0;
//static const float green_range = 5400.0;
//
//static const float model_size = 700.0;
////static const float3 atmos_color = float3(0.3, 0.7, 0.9);
//


//VS_OUTPUT vsmain(VS_INPUT input)
//{
//	VS_OUTPUT output = (VS_OUTPUT)0;
//
//	output.position = input.position;
//
//	//WORLD SPACE
//	float3 world_pos = mul(mul(output.position, m_world), m_global);
//	float3 sun_pos = world_pos + normalize(-m_global_light_dir.xyz) * model_size;
//	//VIEW SPACE
//	output.position = mul(world_pos, m_view);
//	//SCREEN SPACE
//	output.position = mul(output.position, m_proj);
//
//	float3 light_direction = normalize(m_global_light_dir.xyz);
//	float sun_specular = max(0.0, dot(input.normal, light_direction));
//
//	output.color = sun_specular;
//
//	return output;
//
//}

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
	float3 normal: NORMAL0;
	float3 world_pos: POSITION0;
	float3 sun_pos: POSITION1;
	float3 light_dir: NORMAL1;
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



static const float model_size = 350.0;
//static const float3 atmos_color = float3(0.3, 0.7, 0.9);

VS_OUTPUT vsmain(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;
	//output.normal = input.normal;

	//WORLD SPACE
	output.position = mul(mul(output.position, m_world), m_global);
	output.world_pos = output.position;
	output.sun_pos = output.world_pos + normalize(-m_global_light_dir.xyz) * model_size;
	//VIEW SPACE
	output.position = mul(output.position, m_view);
	//SCREEN SPACE
	output.position = mul(output.position, m_proj);


	output.light_dir = normalize(m_global_light_dir.xyz);

	return output;

}
