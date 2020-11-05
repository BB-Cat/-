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
	float3 shadowpos: TEXCOORD1;
	float3 shadowpos2: TEXCOORD2;
	float3 shadowpos3: TEXCOORD3;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
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

#define NUM_SHADOW_MAPS (3)
cbuffer constant: register(b3)
{
	float4 m_active_proj;
	row_major float4x4 m_light_world;
	row_major float4x4 m_light_view;
	row_major float4x4 m_light_proj[NUM_SHADOW_MAPS];
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
	output.normal = mul(float4(input.normal.xyz, 0), m_global).xyz;


	//WORLD SPACE
	output.position = mul(mul(output.position, m_world), m_global);
	output.world_pos = output.position;
	output.direction_to_camera = normalize(output.position.xyz - m_camera_position.xyz);
	//VIEW SPACE
	output.position = mul(output.position, m_view);
	//SCREEN SPACE
	output.position = mul(output.position, m_proj);


	//SHADOWMAP SPACE
	float4 shadowpos, shadowpos2, shadowpos3;
	shadowpos.xyz = output.world_pos;
	shadowpos.w = 1;
	shadowpos = mul(shadowpos, m_light_view);

	//split into different positions for each maps' projection matrix
	shadowpos3 = mul(shadowpos, m_light_proj[2]);
	shadowpos2 = mul(shadowpos, m_light_proj[1]);
	shadowpos = mul(shadowpos, m_light_proj[0]);

	shadowpos /= shadowpos.w;
	shadowpos2 /= shadowpos2.w;
	shadowpos3 /= shadowpos3.w;

	//SHADOWMAP TEXTURE COORDS
	shadowpos.y = -shadowpos.y;
	shadowpos2.y = -shadowpos2.y;
	shadowpos3.y = -shadowpos3.y;
	shadowpos.xy = 0.5f * shadowpos.xy + 0.5f;
	shadowpos2.xy = 0.5f * shadowpos2.xy + 0.5f;
	shadowpos3.xy = 0.5f * shadowpos3.xy + 0.5f;


	output.shadowpos = shadowpos;
	output.shadowpos2 = shadowpos2;
	output.shadowpos3 = shadowpos3;

	output.texcoord = input.texcoord;
	return output;

}
