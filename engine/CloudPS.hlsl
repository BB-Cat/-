Texture3D Texture: register(t0);
SamplerState TextureSampler: register(s0);

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

//cloud property settings
cbuffer constant: register(b6)
{
	float4	 m_cloud_position;
	float4	 m_cloud_size;
	float4   m_sampling_resolution;
	float4   m_sampling_weight;
	float4   m_move_dir;
	float4   m_phase_parameters;
	float4   m_detail_sampling_weight;
	float	 m_cloud_density;
	float	 m_vertical_fade;
	float	 m_horizontal_fade;
	float	 m_per_pixel_fade_threshhold;
	float    m_per_sample_fade_threshhold;
	float	 m_in_scattering_strength;
	float	 m_out_scattering_strength;
	float	 m_time;
	float    m_speed;

	// NEW

	float  m_ray_offset_strength;
	float  m_density_offset;
	float  m_detail_noise_scale;
	float  m_detail_speed;
	float  m_detail_noise_weight;
	float  m_light_stepcount;
	float  m_darkness_threshold;
	float  m_light_absorption_towards_sun;
	float  m_light_absorption_through_cloud;
}

static const int MAX_STEPS = 100;

//the fade mod returns a value from 1.0 (no fade) to 0 (fully faded) based on distance from the origin of the cloud
float fadeMod(float3 pos, float3 obj_pos, float3 obj_size, float3 fade_dist)
{
	//check how far the point is from the core of the cloud
	float3 relative_pos = pos - obj_pos;
	//the cloud will only start to fade if the distance exceeds the vfade or wfade threshhold
	float3 fade_weight = abs(relative_pos) - fade_dist;
	fade_weight = fade_weight / (m_cloud_size.xyz / 2.0 - fade_dist);
	fade_weight = 1.0 - fade_weight;


	//combine the results (the Y value is being factored twice because it has more weight than the xz axis in defining cloud shape)
	return min(fade_weight.x * fade_weight.y * fade_weight.y * fade_weight.z, 1);

	return 1;
}

float raymarchCloud(float3 pos, float3 dir)
{
	float density = 0;

	float maxlen = length((m_cloud_position.xyz - m_cloud_size.xyz) - (m_cloud_position.xyz + m_cloud_size.xyz));
	float step_size = maxlen / 150;
	float step_size_max = maxlen / MAX_STEPS;
//------------------------------------------------------------------------------------//
//  calculate how many steps it will take to get through the cloud from this angle.
//------------------------------------------------------------------------------------//
	//add a large number that is bigger than any step count we would possibly use.
	int possible_steps_x = 1000; 
	int possible_steps_z = 1000;
	int possible_steps_y = 1000;

	if (dir.x != 0)
	{
		bool right = dir.x > 0;
		//if the direction is moving right, find the right edge of the box.  if the direction is moving left, find the left edge of the box.
		float target_x_edge = (m_cloud_position.x - m_cloud_size.x / 2.0) * (!right) + (m_cloud_position.x + m_cloud_size.x / 2.0) * (right);
		float possible_width = target_x_edge - pos.x;
		//find the number of steps it will take to hit the side of the box
		possible_steps_x = floor(possible_width / (dir.x * step_size));
	}

	if (dir.y != 0)
	{
		bool up = dir.y > 0;
		//if the direction is moving up, find the top edge of the box.  if the direction is moving down, find the bottom edge of the box.
		float target_y_edge = (m_cloud_position.y - m_cloud_size.y / 2.0) * (!up) + (m_cloud_position.y + m_cloud_size.y / 2.0) * (up);
		float possible_height = target_y_edge - pos.y;
		//find the number of steps it will take to hit the top/bottom of the box
		possible_steps_y = floor(possible_height / (dir.y * step_size));
	}

	if (dir.z != 0)
	{
		bool forward = dir.z > 0;
		////if the direction is moving forward, find the back edge of the box.  if the direction is moving backward, find the front edge of the box.
		float target_z_edge = (m_cloud_position.z - m_cloud_size.z / 2.0) * (!forward) + (m_cloud_position.z + m_cloud_size.z / 2.0) * (forward);
		float possible_depth = target_z_edge - pos.z;
		//find the number of steps it will take to hit the front/back of the box
		possible_steps_z = floor(possible_depth / (dir.z * step_size));
	}

	//use whichever direction will leave the box in less steps.
	int num_steps = (possible_steps_x) * (possible_steps_x <= possible_steps_y && possible_steps_x <= possible_steps_z) + 
		(possible_steps_y) * (possible_steps_y < possible_steps_x && possible_steps_y <= possible_steps_z) +
		(possible_steps_z) * (possible_steps_z < possible_steps_x && possible_steps_z < possible_steps_y);

	if (num_steps > MAX_STEPS)
	{
		step_size = step_size_max;
		num_steps = MAX_STEPS;
	}
//------------------------------------------------------------------------------------------
//		precalculate the fade distance for the cloud
//------------------------------------------------------------------------------------------
	
	float3 fade_dist = float3(m_cloud_size.x * m_horizontal_fade, m_cloud_size.y * m_vertical_fade, m_cloud_size.z * m_horizontal_fade) / 2.0;

//---------------------------------------

	float3 p = pos;
	float sample_vp, sample_v_l, sample_v_m, sample_v_s, mix_sample;
	float concentration;

	////[unroll]
	for (int i = 0; i < num_steps; i++)
	{
		//sample the cloud
		/* because this loop's size depends on a dynamic step count, it cannot be unrolled. */
		/* hlsl cannot determine mipmap level inside an unrolled loop so we have to set the mipmap manually */
		sample_vp =	 Texture.SampleLevel(TextureSampler, (p + m_move_dir.xyz * m_time * m_speed) / m_sampling_resolution.x, 0).x 
			* m_sampling_weight.x;
		sample_v_l = Texture.SampleLevel(TextureSampler, (p + m_move_dir.xyz * m_time * m_speed) / m_sampling_resolution.y, 0).y
			* m_sampling_weight.y;
		sample_v_m = Texture.SampleLevel(TextureSampler, (p + m_move_dir.xyz * m_time * m_speed) / m_sampling_resolution.z, 0).z
			* m_sampling_weight.z;
		sample_v_s = Texture.SampleLevel(TextureSampler, (p + m_move_dir.xyz * m_time * m_speed) / m_sampling_resolution.w, 0).w
			* m_sampling_weight.w;

		mix_sample = (sample_vp + sample_v_l + sample_v_m + sample_v_s);

		mix_sample *= fadeMod(p, m_cloud_position.xyz, m_cloud_size.xyz, fade_dist);

		mix_sample *= (mix_sample > m_per_sample_fade_threshhold);

		density += mix_sample * m_cloud_density;
		if (density >= 1.0) break;

		p += dir * step_size;
	}

	return density * (density > m_per_pixel_fade_threshhold);
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 dir = normalize(input.world_pos.xyz - m_camera_position.xyz);
	float3 pos = float3(input.world_pos.xyz);


	float noise = raymarchCloud(pos, dir);
	return float4(1, 1, 1, noise);
}