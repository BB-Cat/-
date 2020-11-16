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

//===============================================================//
//	Noise Generation Functions
//===============================================================//
//noise settings
cbuffer constant: register(b5)
{
	float4 m_noise_type;
	float4 m_rgba;

	float  m_vor_octaves;
	float  m_vor_frequency;
	float  m_vor_gain;
	float  m_vor_lacunarity;
	float  m_vor_amplitude;
	float  m_vor_cell_size;

	float  m_per_octaves;
	float  m_per_frequency;
	float  m_per_gain;
	float  m_per_lacunarity;
	float  m_per_amplitude;
	float  m_per_cell_size;
}

////cloud property settings
//cbuffer constant: register(b6)
//{
//
//	float4	 m_cloud_position;
//	float4	 m_cloud_size;
//	float4   m_sampling_resolution;
//	float4   m_sampling_weight;
//	float4   m_move_dir;
//	float	 m_cloud_density;
//	float	 m_vertical_fade;
//	float	 m_horizontal_fade;
//	float	 m_per_pixel_fade_threshhold;
//	float    m_per_sample_fade_threshhold;
//	float	 m_in_scattering_strength;
//	float	 m_out_scattering_strength;
//	float	 m_time;
//	float    m_speed;
//}

float stratus_height = 3150.0;

//returns the xyz position of the cloud and the cloud concentration in w
float4 getStratus(float3 pos, float3 dir)
{
	//get the position in the cloud layers that the eye is looking at
	//float stratus_relative_height = m_camera_position.y + stratus_height;
	float steps_to_stratus = stratus_height / dir.y;
	float3 stratus_pos = dir * steps_to_stratus;
	float3 move_mod = float3(m_move_dir.x * m_time * m_speed, 0, m_move_dir.z * m_time * m_speed);

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

float cumulus_height = 3150.0;

//returns the xyz position of the cloud and the cloud concentration in w
float4 getCumulus(float3 pos, float3 dir)
{
	//get the position in the cloud layers that the eye is looking at
	float steps_to_cumulus = cumulus_height / dir.y;
	float3 cumulus_pos = dir * steps_to_cumulus;// +float3(2000, 0, 2000);
	float3 move_mod = float3(m_move_dir.x * m_time * m_speed, 0, m_move_dir.z * m_time * m_speed);

	float3 value = (float3(cumulus_pos.x, 0, cumulus_pos.z) + move_mod) / m_per_cell_size.x;

	float noise = min(brownianPerlin(value, m_per_octaves, m_per_gain, m_per_lacunarity, m_per_amplitude), 1);

	//erode the number of clouds in the sky
	float erosion = min(((noise - 0.3) / 0.3), 1);
	noise *= erosion;

	//reduce the cloud transparency on the horizon so they fade into the distance
	float cloud_transparency = min(((dir.y - 0.15) / 0.15), 1);
	noise *= cloud_transparency;

	return float4(cumulus_pos, noise);
}

//===============================================================//
//   Cloud raymarching
//===============================================================//
static const float base_num_steps = 20;
static const float step_size = 0.4f;

float raymarchCloud(float3 pos, float3 dir, float cloud_presence)
{
	float density = 0;
	dir = normalize(dir);

	float3 p = normalize(float3(pos.x, pos.y, pos.z)) * 10;
	//float3 p = 0;
	float sample_vp, sample_v_l, sample_v_m, sample_v_s, mix_sample;
	float concentration;

	float3 move_mod = float3(m_move_dir.x * m_time * m_speed, 0, m_move_dir.z * m_time * m_speed);
	float transparency_mod = m_cloud_density * (cloud_presence - m_per_pixel_fade_threshhold) / (1 - m_per_pixel_fade_threshhold);
	//float3 move_mod = 0;
	//[unroll]
	//int num_steps = floor(base_num_steps * cloud_presence);
	[unroll]
	for (int i = 0; i < base_num_steps; i++)
	{
		//sample the cloud
		sample_vp = Texture.SampleLevel(TextureSampler,  (p + move_mod) / m_sampling_resolution.x, 0).x	* m_sampling_weight.x;
		sample_v_l = Texture.SampleLevel(TextureSampler, (p + move_mod) / m_sampling_resolution.y, 0).y	* m_sampling_weight.y;
		sample_v_m = Texture.SampleLevel(TextureSampler, (p + move_mod) / m_sampling_resolution.z, 0).z	* m_sampling_weight.z;
		sample_v_s = Texture.SampleLevel(TextureSampler, (p + move_mod) / m_sampling_resolution.w, 0).w	* m_sampling_weight.w;

		mix_sample = (sample_vp + sample_v_l + sample_v_m + sample_v_s);
		mix_sample *= (mix_sample > m_per_sample_fade_threshhold);

		density += mix_sample * transparency_mod;// *cloud_presence;

		p += dir * step_size;
	}


	//return (density * cloud_presence) * ((density * cloud_presence) > m_per_pixel_fade_threshhold); //(density * cloud_presence) * ((density * cloud_presence) > m_per_pixel_fade_threshhold);
	return density * cloud_presence;
}
//===============================================================//
//===============================================================//

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
	float cloud2_color = 0;
	float4 cloud = 0;
	float4 cloud2 = 0;
	if (n.y > 0.15)
	{
		cloud = getStratus(input.world_pos, n);
		if (cloud.w > m_per_pixel_fade_threshhold)
		{
			cloud_color = raymarchCloud(cloud.xyz, n, cloud.w);
		}

		//cloud2 = getCumulus(input.world_pos, n);
		//if (cloud_color < 1.0 && cloud2.w > m_per_pixel_fade_threshhold)
		//{
		//	cloud2_color = raymarchCloud(cloud2.xyz, n, cloud2.w);
		//
		//}
	}

	//=================================================================


	//return float4(1, 1, 1, cloud_color + cloud2_color);
	float4 final_color = float4(final_sky_color.xyz + float3(1,1,1) * (cloud_color + cloud2_color), 1);
	return final_color;
	//float3 final_color = cloud.w;
	//return float4(final_color, 1);

}
