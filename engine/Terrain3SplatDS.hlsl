Texture2D HeightMapR: register(t0);
SamplerState HeightMapSampler: register(s0);
Texture2D HeightMapG: register(t1);
Texture2D HeightMapB: register(t2);
Texture2D HeightMapW: register(t3);

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

//setting for how much to adjust height from the heightmap sample
cbuffer constant: register(b2)
{
	float m_height;
}

struct DS_OUTPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 binormal : NORMAL1;
	float3 tangent : NORMAL2;
	float4 texbias : WEIGHTS;
	float3 direction_to_camera : NORMAL3;
	float4 lightcolor: TEXCOORD2;
	float cliff_face : WEIGHTS1;
	float2 cliff_texcoord : TEXCOORD3;
};

// 出力制御点
struct HS_CONTROL_POINT_OUTPUT
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
};

// 出力パッチ定数データ。
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // たとえば、クワド ドメインの [4] になります
	float InsideTessFactor : SV_InsideTessFactor; // たとえば、クワド ドメインの Inside[2] になります
};

//amount to water down the heightmap changes
static float const_erosion = 50.0f;
//bias value at which cliff texture begins to be added to result
static const float cliff_threshhold = 0.6f;
//value at which the result is fully a cliff
static const float max_cliff = 0.55f;
//maximum value at which bump mapping occurs
static const float max_bump_range = 100;

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

	Output.binormal =
		patch[0].binormal * domain.x + patch[1].binormal * domain.y + patch[2].binormal * domain.z;

	Output.tangent =
		patch[0].tangent * domain.x + patch[1].tangent * domain.y + patch[2].tangent * domain.z;


	float4 texbias =
		patch[0].texbias * domain.x + patch[1].texbias * domain.y + patch[2].texbias * domain.z;

	float4 pos = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;

	// Interpolate patch attributes to generated vertices.
	Output.position = domain.x * patch[0].position + domain.y * patch[1].position + domain.z * patch[2].position;

	Output.normal =
		patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z;


	Output.texcoord = domain.x * patch[0].texcoord + domain.y * patch[1].texcoord + domain.z * patch[2].texcoord;
	Output.cliff_texcoord = domain.x * patch[0].cliff_texcoord + domain.y * patch[1].cliff_texcoord + domain.z * patch[2].cliff_texcoord;

	// Interpolating normal can unnormalize it, so normalize it.
	Output.normal = normalize(Output.normal);


	// Choose the mipmap level based on distance to the eye; specifically, choose
	// the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
	//const float MipInterval = 5.0f;
	//float mipLevel = clamp((distance(Output.position.xyz, m_camera_position) - MipInterval) / MipInterval, 0.0f, 12.0f);
	float mipLevel = clamp(distance(Output.position.xyz, m_camera_position) / 10 - 6.0, 0.0, 6.0);

	// Sample height map.
	float h = HeightMapR.SampleLevel(HeightMapSampler, Output.texcoord, 0).x * texbias.r;
	float h2 = HeightMapG.SampleLevel(HeightMapSampler, Output.texcoord, 0).x * texbias.g;
	float h3 = HeightMapB.SampleLevel(HeightMapSampler, Output.texcoord, 0).x * texbias.b;

	//Sample the cliff face
	//float h4 = HeightMapW.SampleLevel(HeightMapSampler, Output.texcoord * 2, 0).x * -8;
	float3 triplanar_balance = abs(Output.normal);
	triplanar_balance = pow(max(triplanar_balance, 0), 2);
	float2 cliff_coord1 = Output.position.zx / 10;  // / triplanar_balance.y;
	float2 cliff_coord2 = Output.position.xy / 10;  // / triplanar_balance.z;
	float2 cliff_coord3 = Output.position.zy / 10;  // / triplanar_balance.x;
	float3 h_sample1 = (HeightMapW.SampleLevel(HeightMapSampler, cliff_coord1, 0).x - 0.5) * 8;
	float3 h_sample2 = (HeightMapW.SampleLevel(HeightMapSampler, cliff_coord2, 0).x - 0.5) * 8;
	float3 h_sample3 = (HeightMapW.SampleLevel(HeightMapSampler, cliff_coord3, 0).x - 0.5) * 8;

	float3 h4 = h_sample1 * triplanar_balance.y + h_sample2 * triplanar_balance.z + h_sample3 * triplanar_balance.x;
	//float h4 = 0;

	Output.texbias = float4(h, h2, h3, texbias.w);
	//find the biased offset between the 3 main terrain types
	float offset = h * (h >= h2 && h >= h3) + h2 * (h2 > h && h2 > h3) + h3 * (h3 > h && h3 >= h2);
	//water down the offset before involving cliff calculation
	offset /= const_erosion;

	//if the texbias for this face is over the cliff threshhold, output a cliff face value to add it to the result
	float cliff_face = min(max((cliff_threshhold - texbias.w) / (cliff_threshhold - max_cliff), 0), 1);
	Output.cliff_face = cliff_face;
	//adjust the offset
	float camera_dist = length(m_camera_position.xyz - Output.position.xyz);
	//offset = (offset * (1.0 - cliff_face) + h4 * cliff_face) * max(((max_bump_range - camera_dist) / max_bump_range), 0);
	offset = (offset * (cliff_face < 0.7) + h4 * (cliff_face >= 0.7)) * max(((max_bump_range - camera_dist) / max_bump_range), 0);




	// Offset vertex along normal.
	Output.position.xyz = Output.position.xyz + (m_height * offset) * Output.normal;
	//establish direction to camera
	Output.direction_to_camera = normalize(Output.position.xyz - m_camera_position.xyz);
	Output.world_pos = Output.position;



	// Project to screen space.
	Output.position = mul(Output.position, m_view);
	Output.position = mul(Output.position, m_proj);


	//Output.dot_cam = dot(Output.direction_to_camera, Output.normal);

	return Output;
}
