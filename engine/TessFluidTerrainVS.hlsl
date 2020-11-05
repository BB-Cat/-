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
	float2 texcoord2: TEXCOORD3;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 binormal : NORMAL1;
	float3 tangent : NORMAL2;
	float3 direction_to_camera : NORMAL3;
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

//constant buffer for time
cbuffer constant: register(b3)
{
	float m_time;
	float m_elapsed;
}

static const float MinTess = 1;
static const float MaxTess = 8;
static const float MinTessRange = 100.0f;
static const float MaxTessRange = 1.0f;

static const float TexScale = 3.0f;

VS_OUTPUT vsmain(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;

	//////WORLD SPACE
	output.position = mul(mul(output.position, m_world), m_global);

	//texture coordinates
	output.texcoord = (float2((output.position.x), (output.position.z)) / 20) + float2(m_time * 0.1f, m_time * 0.15f);
	output.texcoord2 = (float2((output.position.x), (output.position.z)) / 15) + float2(m_time * 0.25f, m_time * 0.075f);
	//calculate tesselation amount
	float d = distance(output.position.xyz, m_camera_position.xyz);
	//d = d * d / 2;
	float tess = sqrt(saturate((MinTessRange - d) / (MinTessRange - MaxTessRange)));
	output.tessfactor = MinTess + tess * (MaxTess - MinTess);

	//normal calculation
	//output.normal = float3(0,1,0);
	//output.normal = mul(float4(output.normal.xyz, 0), m_global).xyz;

	float3 vN = float3(0, 1, 0);
	float3 vB = { 1, 0, -0.001f };
	float3 vT;
	vB = normalize(vB);
	vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));

	output.binormal = vB;
	output.tangent = vT;

	return output;
}