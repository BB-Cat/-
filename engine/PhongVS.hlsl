
#include "functions.fx"
//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


Texture2D EnviromentTexture : register(t1);
SamplerState EnviromentSampler : register(s1);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	matrix  World;
	matrix	matWVP;
};

cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4	AmbientColor;	//環境光
	float4  EyePos;			//カメラ座標
};

static const float3 fogColor = { 0.8,0.8,0.6 };
static const float fogNear = 20.0;
static const float fogFar = 100.0;


//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
//struct VSInput
//{
//	float3 Position : POSITION;
//	float3 Normal   : NORMAL;
//	float2 Tex      : TEXCOORD;
//	float4 Color    : COLOR;	//頂点カラー無し
//};
struct VSINPUT
{
	float4 position: POSITION0;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float4 boneweights: WEIGHTS;
	uint4  boneindices: BONES;
};



//バーテックスシェーダー出力構造体
//struct PSInput
//{
//	float4 Position : SV_POSITION;
//	float4 Color : COLOR0;
//	float3 wNormal : TEXCOORD1;	//ワールド法線
//	float3 wPos : TEXCOORD2;	//ワールド座標
//	float2 Tex : TEXCOORD3;
//};
struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
	float4 bonecolor: WEIGHTS;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
PSInput VSMain(VSInput input)
{
	PSInput output = (PSInput)0;
	float4 P = float4(input.Position, 1.0);

	output.Position = mul(matWVP, P);

	float3 wPos = mul(World, P).xyz;

	float3 wN = mul((float3x3)World, input.Normal).xyz;
	wN = normalize(wN);

	output.Color = input.Color;

	//テクスチャー座標
	output.Tex = input.Tex;
	output.wNormal = wN;
	output.wPos = wPos;

	return output;
}




