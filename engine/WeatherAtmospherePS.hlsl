//Texture3D Texture: register(t0);
//SamplerState TextureSampler: register(s0);
#include "noise.fx"
#include "cloud.fx"


struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 normal: NORMAL0;
	float3 world_pos: POSITION0;
	float3 sun_pos: POSITION1;
	float3 light_dir: NORMAL1;
};

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

//===============================================================//
//	Atmosphere constants
//===============================================================//

static const float atmosphere_height = 300.0;
static const float atmosphere_max_dist = 6200.0 * 2; //more accurately 6189.8

static const float blue_range = 3800.0 * 2;
static const float green_range = 5400.0 * 2;

static const float night_max = -0.2;

//===============================================================//
//===============================================================//

float stratus_height = 3150.0;

//returns the xyz position of the cloud and the cloud concentration in w
float4 getStratus(float3 pos, float3 dir)
{
	float time = m_time * m_speed * m_speed;

	//get the position in the cloud layers that the eye is looking at
	//float stratus_relative_height = m_camera_position.y + stratus_height;
	float steps_to_stratus = stratus_height / dir.y;
	float3 stratus_pos = dir * steps_to_stratus;
	float3 move_mod = m_move_dir * time;//float3(m_move_dir.x * m_time * m_speed, 0, m_move_dir.z * m_time * m_speed);

	float3 value = (float3(stratus_pos.x, 0, stratus_pos.z) + move_mod) / m_per_cell_size.x;

	float noise = min(brownianPerlin(value, m_per_octaves, m_per_gain, m_per_lacunarity, m_per_amplitude), 1);

	//erode the number of clouds in the sky
	float erosion = min(((noise - 0.3) / 0.3), 1);
	noise *= erosion;

	//reduce the cloud transparency on the horizon so they fade into the distance
	float cloud_transparency = min(((dir.y - 0.15) / 0.15), 1);
	noise *= cloud_transparency;

	return float4(stratus_pos, noise);
}

float4 psmain(PS_INPUT input) : SV_TARGET
{

	//=================================================================
	//   Sky color calculation
	//=================================================================
	float3 n = normalize(input.world_pos - m_camera_position);
	float sun_spec = pow(max(0.0, dot(n, input.light_dir)), 1500);
	float3 sun_color = float3(4.0, 2.0, 1.0);

	float3 sun_ref_pos = input.light_dir * float3(atmosphere_max_dist / 2, atmosphere_height, atmosphere_max_dist / 2);
	float3 sky_ref_pos = n * float3(atmosphere_height, atmosphere_height, atmosphere_height);

	float dist = length(sky_ref_pos - sun_ref_pos);
	float r = 1;
	float g = max((green_range - dist) / (green_range / 2), 0);
	float b = max((blue_range - dist) / (blue_range / 2.5), 0);

	float3 sky = normalize(float3(r, g, b));

	float night = min(max(input.light_dir.y, night_max), 0) / night_max;
	float3 night_color = float3(0.05, 0.1, 0.2);

	//add fake mie scattering
	//dot compared to up approaches 0 and dot compared to sun approaches -1 == full effect 
	float mie_amount = max(pow(1 - dot(n, float3(0, 1, 0)), 5) * min(dot(n, input.light_dir) + 0.5, 1), 0);
	float3 mie = float3(1, 0.5, 0.5) * mie_amount;
	mie.r = min(mie.r, 0.5);
	mie.g = min(mie.g, 0.5);
	mie.b = min(mie.b, 0.5);

	float field_color = min(max(dot(n, float3(0, -1, 0)), 0) / 0.3, 1);
	float3 gray = float3(0.35, 0.3, 0.3);

	float3 final_sky_color = ((sky + sun_spec * sun_color + mie) * (1 - field_color) + gray * field_color) * (1 - night) + night_color * night;
	//=================================================================
	//	Cloud retrieval
	//=================================================================
	float cloud_color = 0;
	float4 cloud = 0;
	//variables for marching
	float transmittance = 1;
	float3 light_energy = 0;

	if (n.y > 0.15)
	{
		cloud = getStratus(input.world_pos, n);
		if (cloud.w > m_per_pixel_fade_threshhold)
		{
			//cloud_color = raymarchCloud(cloud.xyz, n, cloud.w);

				// Create ray
			float3 ray_pos = cloud.xyz;
			float3 ray_dir = n;

			// Get cloud box information
			float3 bounds_min = float3(-2000, cloud.y, -2000);
			float3 bounds_max = float3(2000, cloud.y + 330, 2000);

			//float3 out_pos = ray_pos + 330 * ray_dir;
			//float3 bounds_min = float3(min(out_pos.x, ray_pos.x), min(out_pos.y, ray_pos.y), min(out_pos.z, ray_pos.z));
			//float3 bounds_max = float3(max(out_pos.x, ray_pos.x), max(out_pos.y, ray_pos.y), max(out_pos.z, ray_pos.z));
			float2 ray_to_box_info = rayBoxDst(bounds_min, bounds_max, ray_pos, 1 / ray_dir);
			float dist_to_box = ray_to_box_info.x;
			float dist_inside_box = ray_to_box_info.y;

			//Get point of intersection with the cloud container 
			/*(for now, just world pos. if we use deferred rendering we need to calculate)*/
			float3 entry_point = cloud.xyz;

			//random starting offset (to make low resolution noise look less jagged)
			/* this offset should be changed to offset in 3 dimensions, not one.  right now one offset causes the clouds to look blocky
			in areas where they ray goes directly through one row of voxels. other areas are looking great.*/

			float random_offset = BlueNoise.SampleLevel(TextureSampler, input.position.xy / 1024.0 * 3, 0);
			random_offset *= m_ray_offset_strength;

			//Phase function makes clouds brighter around sun
			float cos_angle = dot(ray_dir, normalize(m_global_light_dir));
			float phase_value = phase(cos_angle);

			//set the distance equal to our random value and declare the travel limit to box size.
			/* for deferred rendering with a depth buffer, we want the smaller value between the depth buffer and the box
			   as in depth - dist_to_box */
			//cloud.w = min(cloud.w, 1);
			//cloud.w = max(cloud.w, 0);
			float dist_travelled = random_offset;// *(dist_inside_box - random_offset)* (1.0 - easeInOut(cloud.w));
			float dist_limit = dist_inside_box;

			//static step size
			const float step_size = 11;

			//float random_offset = 0;
			float dist_frac = 0;
			float offset_strength = m_ray_offset_strength * (m_ray_offset_strength > 0) + 1.0 * !(m_ray_offset_strength > 0);
			float3 current_ray;

			float3 inverse_light_dir = normalize(-m_global_light_dir);

			while (dist_travelled < dist_limit)
			{
				current_ray = ray_dir * dist_travelled;
				ray_pos = entry_point + current_ray;
				float density = sampleDensity(ray_pos, float3(4000, 330, 4000), bounds_min, bounds_max);

				if (density > 0)
				{
					float light_transmittance = lightmarch(ray_pos, inverse_light_dir, bounds_min, bounds_max);
					light_energy += density * step_size * transmittance * light_transmittance * phase_value;
					transmittance *= exp(-density * step_size * m_light_absorption_through_cloud * cloud.w);

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

			float final_coverage = (cloud.w - m_per_pixel_fade_threshhold) / (1.0 - m_per_pixel_fade_threshhold);

			//transmittance = 1.0 - ((1.0 - transmittance) * final_coverage);
			//transmittance = abs(transmittance);
			light_energy *= cloud.w;
			//light_energy = abs(light_energy);
		}

		//if (light_energy)
		//{
		//	light_energy = 1;
		//	transmittance = 0;
		//}
	}
	//=================================================================

	float3 c = light_energy * m_global_light_color;

	//float3 col = final_sky_color.xyz * transmittance + c;
	float3 col = final_sky_color.xyz * transmittance + c;
	//float3 col = c;

	//float3 col = (light_energy > 0 && light_energy < 1);

	return float4(col, 1);
	//cloud.w = clamp(cloud.w, 0, 1.0);
	//return float4(1.0, 1.0, 1.0, cloud.w);

}
