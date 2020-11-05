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

static const float atmosphere_height = 300.0;
static const float atmosphere_max_dist = 6200.0 * 2; //more accurately 6189.8

static const float blue_range = 3800.0 * 2;
static const float green_range = 5400.0 * 2;

static const float night_max = -0.2;

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


	return float4(final_sky_color, 1);
}