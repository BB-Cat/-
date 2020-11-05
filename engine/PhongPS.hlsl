#include "PhongVS.hlsl"

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSMain(PSInput input) : SV_Target
{
	float4 color = DiffuseTexture.Sample(DecalSampler,input.Tex);
	float3 N = normalize(input.wNormal);
	float3 E = normalize(EyePos.xyz - input.wPos);
	float3 L = normalize(LightDir.xyz);
	// 環境光
	float3 A = AmbientColor.rgb;

	//拡散反射
	float3 C = LightColor.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = Diffuse(N, L, C, Kd);

	//鏡面反射
	float3 Ks = float3(1, 1, 1);
	float3 S = BlinnPhongSpecular(N, L, C, E, Ks, 20);

	color *= input.Color * float4(A + D + S, 1.0);
	return color;
}