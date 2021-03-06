
// 入力制御点
struct VS_CONTROL_POINT_OUTPUT
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
	// TODO: 他のスタッフの変更/追加
};


cbuffer constant: register(b0)
{
	float		m_tess_factor;
};




#define NUM_CONTROL_POINTS 3

// パッチ定数関数
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	//// ここにコードを挿入して出力を計算します
	//Output.EdgeTessFactor[0] = m_tess_factor;
	//Output.EdgeTessFactor[1] = m_tess_factor;
	//Output.EdgeTessFactor[2] = m_tess_factor;
	//Output.InsideTessFactor = m_tess_factor; // たとえば、代わりに動的テセレーション係数を計算できます

	Output.EdgeTessFactor[0] = 0.5f * (ip[1].tessfactor + ip[2].tessfactor);
	Output.EdgeTessFactor[1] = 0.5f * (ip[2].tessfactor + ip[0].tessfactor);
	Output.EdgeTessFactor[2] = 0.5f * (ip[0].tessfactor + ip[1].tessfactor);
	Output.InsideTessFactor = Output.EdgeTessFactor[0];

	return Output;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT hsmain(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONTROL_POINT_OUTPUT Output;

	// ここにコードを挿入して出力を計算します
	Output.position = ip[i].position;
	Output.texcoord = ip[i].texcoord;
	Output.world_pos = ip[i].world_pos;
	Output.normal = ip[i].normal;
	Output.binormal = ip[i].binormal;
	Output.tangent = ip[i].tangent;
	Output.texbias = ip[i].texbias;
	Output.direction_to_camera = ip[i].direction_to_camera;
	Output.lightcolor = ip[i].lightcolor;
	Output.cliff_texcoord = ip[i].cliff_texcoord;

	return Output;
}
