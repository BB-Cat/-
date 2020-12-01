#include "cloud.fx"
#include "noise.fx"

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: NORMAL1;
	float3 world_pos: WORLD_POSITION;
};

//world and camera settings
cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_camera_position;
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

	// Create ray
	float3 ray_pos = m_camera_position.xyz;
	float3 ray_dir = normalize(input.world_pos.xyz - m_camera_position.xyz);

	// Get cloud box information
	float3 bounds_min = m_cloud_position.xyz - m_cloud_size.xyz / 2.0;
	float3 bounds_max = m_cloud_position.xyz + m_cloud_size.xyz / 2.0;
	float2 ray_to_box_info = rayBoxDst(bounds_min, bounds_max, ray_pos, 1 / ray_dir);
	float dist_to_box = ray_to_box_info.x;
	float dist_inside_box = ray_to_box_info.y;

	//Get point of intersection with the cloud container 
	/*(for now, just world pos. if we use deferred rendering we need to calculate)*/
	float3 entry_point = input.world_pos.xyz;

	//random starting offset (to make low resolution noise look less jagged)
	/* this offset should be changed to offset in 3 dimensions, not one.  right now one offset causes the clouds to look blocky 
	in areas where they ray goes directly through one row of voxels. other areas are looking great.*/
	float random_offset = BlueNoise.SampleLevel(TextureSampler, input.position.xy / 1024.0 * 3, 0);
	random_offset *= m_ray_offset_strength;
	//EDIT 20/12 -- ADD AN ENTRY POINT OFFSET BASED ON BLUE NOISE
	entry_point += (rand1dTo3d(random_offset) * m_ray_offset_strength * 2) - float3(1, 1, 1);

	//Phase function makes clouds brighter around sun
	float cos_angle = dot(ray_dir, normalize(m_global_light_dir));
	float phase_value = phase(cos_angle);

	//set the distance equal to our random value and declare the travel limit to box size.
	/* for deferred rendering with a depth buffer, we want the smaller value between the depth buffer and the box 
	   as in depth - dist_to_box */
	float dist_travelled = random_offset;
	float dist_limit = dist_inside_box;

	//static step size
	const float step_size = 11;

	//March through the box
	float transmittance = 1;
	float3 light_energy = 0;
	//float random_offset = 0;
	float dist_frac = 0;
	float offset_strength = m_ray_offset_strength * (m_ray_offset_strength > 0) + 1.0 * !(m_ray_offset_strength > 0);
	float3 current_ray;

	float3 inverse_light_dir = normalize(-m_global_light_dir);

	while (dist_travelled < dist_limit) 
	{
		current_ray = ray_dir * dist_travelled;
		ray_pos = entry_point + current_ray;
		float density = sampleDensity(ray_pos, m_cloud_size.xyz, bounds_min, bounds_max);

		if (density > 0) 
		{
			float light_transmittance = lightmarch(ray_pos, inverse_light_dir, bounds_min, bounds_max);
			light_energy += density * step_size * transmittance * light_transmittance * phase_value;
			transmittance *= exp(-density * step_size * m_light_absorption_through_cloud);

			// Exit early if T is close to zero as further samples won't affect the result much
			if (transmittance < 0.01) 
			{
				break;
			}
		}
		//dist_frac = dist_travelled;
		//dist_frac = frac(dist_frac);
		random_offset = BlueNoise.SampleLevel(TextureSampler, (input.world_pos.xz * input.world_pos.y * (m_time % 1000) / 1024.0 * 3) +
			frac(float2(dist_travelled * 12.358, dist_travelled * 34.123) / random_offset), 0);
		random_offset = max(random_offset * offset_strength, 0.5);
		//while (random_offset < 1) random_offset += 1;
		dist_travelled += (step_size * random_offset);
	}

	//Add clouds to background
	float3 background_color = float3(0.3, 0.3, 0.4);
	float3 cloud_color = light_energy * m_global_light_color;
	float3 col = background_color * transmittance + cloud_color;
	return float4(col, 1);
	//return random_offset;

	//return light_energy;
	//return float4(light_energy, 1);


	//float noise = raymarchCloud(pos, dir);
	//return float4(1, 1, 1, noise);
	//return float4(1, 1, 1, 1);
}