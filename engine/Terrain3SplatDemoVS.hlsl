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


cbuffer constant: register(b5)
{
	float m_max_tess;
	float m_min_tess;
	float m_min_tess_range;
	float m_max_tess_range;
}




//static const float MinTess = 1;
//static const float MaxTess = 8;
//static const float MinTessRange = 200.0f;
//static const float MaxTessRange = 120.0f;

VS_OUTPUT vsmain(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;

	//////WORLD SPACE
	output.position = mul(mul(output.position, m_world), m_global);
	output.direction_to_camera = normalize(output.position.xyz - m_camera_position.xyz);

	//texture coordinates
	output.texcoord = float2((output.position.x), (output.position.z)) / 30;
	output.cliff_texcoord = output.texcoord;

	//calculate tesselation amount
	float d = distance(output.position.xyz, m_camera_position.xyz);
	float tess = saturate((m_min_tess_range - d) / (m_min_tess_range - m_max_tess_range));
	output.tessfactor = m_min_tess + tess * (m_max_tess - m_min_tess);
	output.tessfactor = (int)(output.tessfactor);

	output.texbias = input.boneweights;

	//calculate the binormal and tangent 
	float3 vN = input.normal;
	float3 vB = { 1, 0, -0.001f };
	float3 vT;

	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	output.binormal = vB;
	output.tangent = vT;
	output.normal = input.normal;

	return output;
}