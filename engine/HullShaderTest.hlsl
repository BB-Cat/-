// 入力制御点
struct HSInput
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 direction_to_camera : NORMAL1;
	float4 lightcolor: TEXCOORD2;
};

// 出力制御点
struct DSInput
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 direction_to_camera : NORMAL1;
	float4 lightcolor: TEXCOORD2;
};

// 出力パッチ定数データ。
struct HSConstantOutput
{
	float EdgeTessFactor[3]			: SV_TessFactor; // たとえば、クワド ドメインの [4] になります
	float InsideTessFactor			: SV_InsideTessFactor; // たとえば、クワド ドメインの Inside[2] になります
	// TODO: 他のスタッフの変更/追加

};

#define NUM_CONTROL_POINTS 3

// パッチ定数関数
HSConstantOutput CalcHSPatchConstants(
	InputPatch<HSInput, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	//HS_CONSTANT_DATA_OUTPUT Output;

	//// ここにコードを挿入して出力を計算します
	//Output.EdgeTessFactor[0] = 
	//	Output.EdgeTessFactor[1] = 
	//	Output.EdgeTessFactor[2] = 
	//	Output.InsideTessFactor = 15; // たとえば、代わりに動的テセレーション係数を計算できます

	//return Output;

	HSConstantOutput output;
	output.factor[0] = TessFactor;
	output.factor[1] = TessFactor;
	output.factor[2] = TessFactor;
	output.inner_factor = TessFactor;

	return output;

}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
DSInput hsmain( 
	InputPatch<HSInput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	//HS_CONTROL_POINT_OUTPUT Output;

	//// ここにコードを挿入して出力を計算します
	//Output.vPosition = ip[i].vPosition;

	//return Output;

	DSInput output = (DSInput)0;
	output.Position = input[cpid].Position;
	output.Color = input[cpid].Color;
	output.Tex = input[cpid].Tex;
	output.Normal = input[cpid].Normal;
	output.wPos = input[cpid].wPos;
	return output;

}
