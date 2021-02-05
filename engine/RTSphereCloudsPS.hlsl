#define _RTCLOUDS

//#include "cloud.fx"
#include "noise.fx"
#include "raymarch.fx"

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
};

////world and camera settings
//cbuffer constant: register(b0)
//{
//	row_major float4x4 m_world;
//	row_major float4x4 m_view;
//	row_major float4x4 m_proj;
//	float4 m_camera_position;
//};

////scene global light settings
//cbuffer constant: register(b2)
//{
//	float3  m_global_light_dir;
//	float	m_global_light_strength;
//	float3  m_global_light_color;
//	float3  m_ambient_light_color;
//}

#define FULLSTEPS (40.0)

float4 psmain(PS_INPUT input) : SV_TARGET
{
	Ray ray;
	ray.origin = m_camera_position.xyz;
	ray.direction = input.world_pos.xyz - m_camera_position.xyz;
	ray.direction = normalize(ray.direction);
	ray.energy = float3(1, 1, 1);


	//float4 spheres[MAX_SPHERES];
	//spheres[0] = float4(0, 0, 0, 100);
	//spheres[1] = float4(100, 10, 5, 80);
	//spheres[2] = float4(200, 10, 5, 80);
	//spheres[3] = float4(-100, -70, 70, 80);
	//spheres[4] = float4(0, 200, -40, 80);

	RayHit hit = trace(ray);

	float3 col = 0;
	float a = 0.0;

	if (!(hit.distance > MAX_DIST))
	{
		////TODO: i need to move the density limit and cloud amount calculation 
		////into the trace function so can break the second it hits 1
		//float density_limit = 100;
		//float cloud_amount = min(1, hit.total_through / density_limit);
		//int stepcount = floor(cloud_amount * FULLSTEPS);

		//float3 pos = hit.position.xyz;
		//float3 end = ray.origin + ray.direction * hit.far_distance;
		//float step_size = length(end - pos) / stepcount;

		//float total = 0;
		//for (int i = 0; i < stepcount; i++)
		//{
		//	total += 0.06 * Texture.SampleLevel(TextureSampler, pos / 200, 0).x;
		//	pos += ray.direction * step_size;
		//}
		//col = beer(total);

		float3 c = hit.total_through;
		float a = 1;
		//TEMPORARY
		if (m_params.w >= 3)
		{
			c = hit.distance / 2000.0;
			a = 1;
		}
		else if (m_params.w >= 2)
		{
			c = hit.normal;
			a = 1;
		}
		else if (m_params.w >= 1)
		{
			c = (hit.far_distance - hit.distance) / 600.0;
			a = 1;
		}
		else
		{
			c = 1;
			a = hit.total_through;
		}

		//return float4(c, a + hit.total_through);
		return float4(c, a);
	}
	return float4(col, a);
}