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

[numthreads(1024, 1, 1)]
void CS_main(int3 id : SV_DispatchThreadID)
{
	float2 coords = float2((id.x % m_resolution.x) / m_resolution.x, 1.0 - (id.x / m_resolution.x / m_resolution.y));
	float2 uv = (coords - 0.5) * 2;

	float3 color;
	
	//float3 ro = m_camera_position.xyz;
	////float3 rd = mul(m_world, float4(uv.x * ASPECT, uv.y, 1.0, 0.0f)).xyz;
	//float3 rd = float3(uv.x * ASPECT, uv.y, 1.0);
	//rd = normalize(rd);
	//normalize(float3(uv.x * ASPECT, uv.y, 1.0));

	//float4 res = rayMarchSpheres(ro, rd);
	//float d = res.w;
	//float3 p = ro + rd * d;

	//if (d >= MAX_DIST)
	//{
	//	rd * 0.5f + 0.5f;
	//	float theta = acos(rd.y) / PI;
	//	float phi = atan2(rd.x + 1.0, -rd.z) / PI * 0.5;
	//	color = Texture.SampleLevel(TextureSampler, float2(phi, theta), 0).rgb;
	//	output[id.x].position1 = float4(color, 1);
	//}
	//else
	//{
	//	float diffuse = getLightSpheres(p);
	//	color = saturate(res.rgb * diffuse);

	//	output[id.x].position1 = float4(color, 1);
	//}

	//===============================================--


	Ray ray;
	ray.origin = float3(0, 3.0, 0);
	ray.direction = normalize(float3(uv.x * ASPECT, uv.y - 0.5, 1.0));
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

	//if (result.x < 0)
	//{
	//	ray.direction * 0.5f + 0.5f;
	//	float theta = acos(ray.direction.y) / PI;
	//	float phi = atan2(ray.direction.x + 1.0, -ray.direction.z) / PI * 0.5;
	//	result = Texture.SampleLevel(TextureSampler, float2(phi, theta), 0).rgb;
	//}


	output[id.x].position1 = float4(result, 1);

}