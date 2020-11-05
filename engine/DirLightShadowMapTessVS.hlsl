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
	float2 texcoord: TEXCOORD0;
	float3 normal : NORMAL0;
	float tessfactor : TESSELATION;
};


cbuffer constant: register(b3)
{
	row_major float4x4 m_light_world;
	row_major float4x4 m_light_view;
	row_major float4x4 m_light_proj;
	float4 m_light_camera_position; //in this case, it is the position of the light camera
};

#define MAX_BONES 32 
cbuffer constant: register(b1)
{
	row_major float4x4 m_global;
	row_major float4x4 m_bonetransforms[MAX_BONES];
	row_major float4x4 m_coordinate_system;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;
	output.normal = float3(0, 1, 0);

	//////WORLD SPACE
	output.position = mul(mul(output.position, m_light_world), m_global);

	//texture coordinates
	output.texcoord = float2((output.position.x), (output.position.z)) / 5;

	output.tessfactor = 3;

	return output;

}