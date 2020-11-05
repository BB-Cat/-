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
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 binormal : NORMAL1;
	float3 tangent : NORMAL2;
	float3 direction_to_camera : NORMAL3;
	float4 lightcolor: TEXCOORD2;
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

VS_OUTPUT vsmain(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;

	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (input.boneweights[i] * mul(input.position, m_bonetransforms[input.boneindices[i]])).xyz;
		n += (input.boneweights[i] * mul(float4(input.normal.xyz, 0), m_bonetransforms[input.boneindices[i]])).xyz;
	}
	if (p.x != 0 || p.y != 0 || p.z != 0)
	{
		output.position = float4(p, 1.0f);
		output.normal = float4(n, 0.0f);
	}
	else
	{
		output.position = input.position;
		output.normal = input.normal;
	}

	output.normal = mul(float4(output.normal.xyz, 0), m_global).xyz;

	////WORLD SPACE
	output.position = mul(mul(output.position, m_world), m_global);
	output.world_pos = output.position;
	output.direction_to_camera = normalize(output.position.xyz - m_camera_position.xyz);
	////VIEW SPACE
	output.position = mul(output.position, m_view);
	////SCREEN SPACE
	output.position = mul(output.position, m_proj);

	float3 vN = output.normal;
	float3 vB = { 0, 1, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	output.binormal = vB;
	output.tangent = vT;

	//texture coordinates
	output.texcoord = input.texcoord;

	return output;

}