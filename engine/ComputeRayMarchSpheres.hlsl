#include "raymarch.fx"

//input
struct InputData
{
    float4 position1;
};

//output
struct OutputData
{
    float4 position1;
};

StructuredBuffer<InputData>  input : register(t0);
RWStructuredBuffer<OutputData> output : register(u0);

//‰ü‘¢‚·‚é‚Ü‚Å‚±‚±‚Åaspect ratio‚Ì’è‹`‚ð‚µ‚Ä‚¢‚é
#define ASPECT 1.388
//#define fov 0.9;

[numthreads(1024, 1, 1)]
void CS_main(int3 id : SV_DispatchThreadID)
{
	float2 coords = float2((id.x % m_resolution.x) / m_resolution.x, 1.0 - (id.x / m_resolution.x / m_resolution.y));
	float2 uv = (coords - 0.5) * 2;

	float3 color;

	Ray ray;
	ray.origin = float3(0, 3.0, -1.0);
	ray.direction = float3(uv.x * ASPECT, uv.y - 0.5, 1.0);
	ray.direction = normalize(ray.direction);
	//ray.direction.xy *= fov;
	ray.energy = float3(1, 1, 1);


	//RayHit hit = trace(ray);
	//float3 result = shade(ray, hit);

	// Trace and shade
	float3 result = float3(0, 0, 0);
	for (int i = 0; i < m_params.x; i++)
	{
		RayHit hit = trace(ray);
		result += ray.energy * shade(ray, hit);

		if (!any(ray.energy)) break;
	}

	output[id.x].position1 = float4(result, 1);
	//output[id.x].position1 = float4(1,1,1, 1);
}