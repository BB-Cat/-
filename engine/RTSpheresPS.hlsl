#include "raymarch.fx"

cbuffer constant: register(b7)
{
	float m_time; //millisecond timer
	float m_elapsed; //time since the last frame
};

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
};

float weight(float t, float log2radius, float gamma)
{
	return exp(-gamma * pow(log2radius - t, 2.0));
}

//‰ü‘¢‚·‚é‚Ü‚Å‚±‚±‚Åaspect ratio‚Ì’è‹`‚ð‚µ‚Ä‚¢‚é
#define ASPECT 1.388

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float2 coord = input.texcoord * 2.0 - 1.0;
	//coord.x *= 1.388;
	float2 uv = coord;

	float3 color;

	Ray ray;
	//mul(output.position, m_world)

	float4 wro = mul(float4(uv.x, uv.y, -1, 1), m_inverseVP);
	float4 wrd = mul(float4(uv.x, uv.y, 1, 1), m_inverseVP);

	//ray.origin = wro.xyz / wro.w;
	//ray.direction = wrd.xyz / wrd.w - wro.xyz / wro.w;

	ray.origin = wro.xyz / wro.w;
	ray.direction = wrd.xyz / wrd.w - ray.origin;

	//ray.origin = float3(0, 2.0, -5.0);
	//ray.direction = float3(uv.x * ASPECT, uv.y - 0.5, 2.0);


	ray.direction = normalize(ray.direction);
	ray.energy = float3(1, 1, 1);

	// Trace and shade
	float3 result = float3(0, 0, 0);
	for (int i = 0; i < m_params.x; i++)
	{
		RayHit hit = trace(ray);
		result += ray.energy * shade(ray, hit);

		if (!any(ray.energy)) break;
	}

	////output[id.x].position1 = float4(result, 1);
	return float4(result, 1);
}