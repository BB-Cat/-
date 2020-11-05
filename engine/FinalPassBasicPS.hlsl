Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 diffuse = Texture.Sample(TextureSampler, input.texcoord);

	return float4(diffuse, 1);
	//return float4(1, 1, 1, 1);
}