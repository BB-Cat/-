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
	float4 position: POSITION0;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 direction_to_camera : NORMAL1;
	float4 lightcolor: TEXCOORD2;

	float tessfactor : TESSELATION;
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




static const float MinTess = 1;
static const float MaxTess = 12;
static const float MinTessRange = 45.0f;
static const float MaxTessRange = 15.0f;




VS_OUTPUT vsmain(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;
	//output.normal = input.normal;
	output.normal = float3(0, 1, 0);
	//output.normal = mul(float4(output.normal.xyz, 0), m_global).xyz;

	//////WORLD SPACE
	output.position = mul(mul(output.position, m_world), m_global);
	output.direction_to_camera = normalize(output.position.xyz - m_camera_position.xyz);

	//texture coordinates
	output.texcoord = float2((output.position.x), (output.position.z)) / 5;

	//calculate tesselation amount
	float d = distance(output.position.xyz, m_camera_position.xyz);
	float tess = saturate((MinTessRange - d) / (MinTessRange - MaxTessRange));
	output.tessfactor = MinTess + tess * (MaxTess - MinTess);

	return output;
}