#include "Sampling.fx"

SamplerState TextureSampler: register(s0);

Texture2D T1: register(t0);
Texture2D N1: register(t1);

Texture2D T2: register(t2);
Texture2D N2: register(t3);

Texture2D T3: register(t4);
Texture2D N3: register(t5);

Texture2D T4: register(t6);
Texture2D N4: register(t7);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD1;
	float4 texbias: TEXCOORD2;
	float3 light: NORMAL0;
	float cliff_amount : NORMAL1;
	float3 fog_color: TEXCOORD3;
	float fog_amount : NORMAL2;
};

//scene global light settings
cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}


float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 sample_color = T1.Sample(TextureSampler, input.texcoord);
	float3 sample_color2 = T2.Sample(TextureSampler, input.texcoord);
	float3 sample_color3 = T3.Sample(TextureSampler, input.texcoord);
	float3 sample_color4 = T4.Sample(TextureSampler, input.texcoord * 2);


	//float3 final_sample = sample_color * input.texbias.r + sample_color2 * input.texbias.g + sample_color3 * input.texbias.b;
	float3 final_sample = getTextureSplat(sample_color, sample_color2, sample_color3, input.texbias.rgb);
	//if necessary, mix the cliff texture
	final_sample = final_sample * (1.0 - input.cliff_amount) + sample_color4 * input.cliff_amount;

	float3 final = (final_sample * input.light) * (1 - input.fog_amount) + input.fog_color * input.fog_amount;
	//float3 final = input.light;

	return float4(final, 1);
}