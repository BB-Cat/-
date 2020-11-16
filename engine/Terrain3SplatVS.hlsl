
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
	float3 binormal : NORMAL1;
	float3 tangent : NORMAL2;
	float4 texbias : WEIGHTS;
	float3 direction_to_camera : NORMAL3;
	float4 lightcolor: TEXCOORD2;
	float2 cliff_texcoord : TEXCOORD3;

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
static const float MaxTess = 8;
static const float MinTessRange = 200.0f;
static const float MaxTessRange = 120.0f;




VS_OUTPUT vsmain(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;

	//////WORLD SPACE
	output.position = mul(mul(output.position, m_world), m_global);
	output.direction_to_camera = normalize(output.position.xyz - m_camera_position.xyz);

	//texture coordinates
	output.texcoord = float2((output.position.x), (output.position.z)) / 10;
	//float val = ((output.position.x) + (output.position.z)) % 2 * 10;
	//float val = output.position.x - output.position.z;
	//float val2 = (output.position.y);
	//output.cliff_texcoord = float2(val, val2) / 10;

	//output.cliff_texcoord = floor(output.position.x) / 10.0 + floor(output.position.y) / 10.0 + floor(output.position.z) / 10.0;
	//output.cliff_texcoord = 
	float clifftex_x = floor((output.position.x + output.position.y) / 10.0);
	float clifftex_z = floor((output.position.z + output.position.y) / 10.0);

	output.cliff_texcoord = float2(clifftex_x, clifftex_z);

	//calculate tesselation amount
	float d = distance(output.position.xyz, m_camera_position.xyz);
	float tess = saturate((MinTessRange - d) / (MinTessRange - MaxTessRange));
	output.tessfactor = MinTess + tess * (MaxTess - MinTess);
	output.tessfactor = (int)(output.tessfactor);

	output.texbias = input.boneweights;

	//calculate the binormal and tangent 
	float3 vN = input.normal;
	float3 vB = { 1, 0, -0.001 };
	float3 vT;

	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	output.binormal = vB;
	output.tangent = vT;
	output.normal = input.normal;

	return output;
}