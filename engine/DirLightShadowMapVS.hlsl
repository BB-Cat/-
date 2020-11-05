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
};


#define NUM_SHADOW_MAPS (3)
cbuffer constant: register(b3)
{
	float4 m_active_proj;
	row_major float4x4 m_light_world;
	row_major float4x4 m_light_view;
	row_major float4x4 m_light_proj;
	row_major float4x4 m_light_proj2;
	row_major float4x4 m_light_proj3;

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

	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (input.boneweights[i] * mul(input.position, m_bonetransforms[input.boneindices[i]])).xyz;
	}
	if (p.x != 0 || p.y != 0 || p.z != 0)
	{
		output.position = float4(p, 1.0f);
	}
	else
	{
		output.position = input.position;
	}


	//WORLD SPACE
	output.position = mul(mul(output.position, m_light_world), m_global);
	//VIEW SPACE
	output.position = mul(output.position, m_light_view);
	//SCREEN SPACE
	float4 pos = output.position;
	output.position = 
		mul(output.position, m_light_proj) * (m_active_proj.x == 0)+
		mul(output.position, m_light_proj2) * (m_active_proj.x == 1)+
		mul(output.position, m_light_proj3) * (m_active_proj.x == 2);

	return output;

}