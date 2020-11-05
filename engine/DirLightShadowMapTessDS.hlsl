Texture2D HeightMap: register(t0);
SamplerState HeightMapSampler: register(s0);

#define NUM_SHADOW_MAPS (3)
cbuffer constant: register(b3)
{
	row_major float4x4 m_light_world;
	row_major float4x4 m_light_view;
	row_major float4x4 m_light_proj[NUM_SHADOW_MAPS];
};

#define MAX_BONES 32 
cbuffer constant: register(b1)
{
	row_major float4x4 m_global;
	row_major float4x4 m_bonetransforms[MAX_BONES];
	row_major float4x4 m_coordinate_system;
};

//setting for how much to adjust height from the heightmap sample
cbuffer constant: register(b2)
{
	float m_height;
}

//static const float height = 0.16;

struct DS_OUTPUT
{
	float4 position: SV_POSITION;
};

// 出力制御点
struct HS_CONTROL_POINT_OUTPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal : NORMAL0;
};

// 出力パッチ定数データ。
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // たとえば、クワド ドメインの [4] になります
	float InsideTessFactor : SV_InsideTessFactor; // たとえば、クワド ドメインの Inside[2] になります
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT dsmain(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	//float4 pos = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;

	// Interpolate patch attributes to generated vertices.
	Output.position = domain.x * patch[0].position + domain.y * patch[1].position + domain.z * patch[2].position;
	float3 normal = domain.x * patch[0].normal + domain.y * patch[1].normal + domain.z * patch[2].normal;
	float2 texcoord = domain.x * patch[0].texcoord + domain.y * patch[1].texcoord + domain.z * patch[2].texcoord;

	// Interpolating normal can unnormalize it, so normalize it.
	normal = normalize(normal);

	// Sample height map.
	float h = HeightMap.SampleLevel(HeightMapSampler, texcoord, 0).x;

	// Offset vertex along normal.
	Output.position.xyz = Output.position.xyz + (m_height * (h)) * normal;

	// Project to screen space.
	Output.position = mul(Output.position, m_light_view);
	Output.position = mul(Output.position, m_light_proj[0]);

	return Output;
}
