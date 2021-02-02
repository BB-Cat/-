Texture2D Texture: register(t0);
SamplerState TextureSampler: register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
};

float4 psmain(PS_INPUT input) : SV_TARGET
{

	//float3 tex = Texture.Sample(TextureSampler, input.texcoord).rgb;
	//float4 color = float4(tex.xyz, 1);
	//return color;

	return Texture.Sample(TextureSampler, input.texcoord);
}