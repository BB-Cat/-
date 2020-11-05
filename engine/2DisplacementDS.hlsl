Texture2D HeightMap: register(t0);
SamplerState HeightMapSampler: register(s0);

Texture2D HeightMap2: register(t1);
SamplerState HeightMapSampler2: register(s1);

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

static const float height = 0.26;

struct DS_OUTPUT
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

// 出力制御点
struct HS_CONTROL_POINT_OUTPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float2 texcoord2: TEXCOORD3;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 binormal : NORMAL1;
	float3 tangent : NORMAL2;
	float3 direction_to_camera : NORMAL3;
	float4 lightcolor: TEXCOORD2;
};

// 出力パッチ定数データ。
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // たとえば、クワド ドメインの [4] になります
	float InsideTessFactor : SV_InsideTessFactor; // たとえば、クワド ドメインの Inside[2] になります
	// TODO: 他のスタッフの変更/追加
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT dsmain(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.texcoord =
		patch[0].texcoord * domain.x + patch[1].texcoord * domain.y + patch[2].texcoord * domain.z;

	Output.direction_to_camera =
		patch[0].direction_to_camera * domain.x + patch[1].direction_to_camera * domain.y + patch[2].direction_to_camera * domain.z;

	Output.lightcolor =
		patch[0].lightcolor * domain.x + patch[1].lightcolor * domain.y + patch[2].lightcolor * domain.z;

	//Output.normal =
	//	patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z;

	Output.binormal =
		patch[0].binormal * domain.x + patch[1].binormal * domain.y + patch[2].binormal * domain.z;

	Output.tangent =
		patch[0].tangent * domain.x + patch[1].tangent * domain.y + patch[2].tangent * domain.z;


	//float4 pos = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;

		// Interpolate patch attributes to generated vertices.
	Output.position = domain.x * patch[0].position + domain.y * patch[1].position + domain.z * patch[2].position;

	//Output.normal = domain.x * patch[0].normal + domain.y * patch[1].normal + domain.z * patch[2].normal;
	float3 p1 = patch[0].position;
	float3 p2 = patch[1].position;
	float3 p3 = patch[2].position;

	Output.normal = normalize(cross((p2 - p1), (p3 - p1)));
	


	Output.texcoord = domain.x * patch[0].texcoord + domain.y * patch[1].texcoord + domain.z * patch[2].texcoord;
	//float2 tex2 = domain.x * patch[0].texcoord2 + domain.y * patch[1].texcoord2 + domain.z * patch[2].texcoord2;

	// Interpolating normal can unnormalize it, so normalize it.
	//Output.normal = normalize(Output.normal);

	//
	// Displacement mapping.
	//

	// Choose the mipmap level based on distance to the eye; specifically, choose
	// the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
	//const float MipInterval = 5.0f;
	//float mipLevel = clamp((distance(Output.position.xyz, m_camera_position) - MipInterval) / MipInterval, 0.0f, 12.0f);
	//float mipLevel = clamp(distance(Output.position.xyz, m_camera_position) / 10 - 6.0f, 0.0f, 6.0f);

	// Sample height map (stored in alpha channel).
	//float h = HeightMap.SampleLevel(HeightMapSampler, Output.texcoord, 0).x;
	//float h = HeightMap.SampleLevel(HeightMapSampler, Output.texcoord, 0).x;
	//float h2 = HeightMap2.SampleLevel(HeightMapSampler2, tex2, 0).x;

	// Offset vertex along normal.
	//Output.position.xyz = Output.position.xyz + ((height * (h)) + (6 * (h2))) * float3(0, 1, 0);
	Output.world_pos = Output.position;
	Output.direction_to_camera = normalize(Output.world_pos.xyz - m_camera_position.xyz);

	// Project to homogeneous clip space.
		//pos = mul(pos, m_view);
	Output.position = mul(Output.position, m_view);
	Output.position = mul(Output.position, m_proj);

	//Output.lightcolor = HeightMap.SampleLevel(HeightMapSampler, Output.texcoord, mipLevel);

	return Output;
}
