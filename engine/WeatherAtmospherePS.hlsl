Texture3D Texture: register(t0);
SamplerState TextureSampler: register(s0);

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

//cloud property settings
cbuffer constant: register(b6)
{

	float4	 m_cloud_position;
	float4	 m_cloud_size;
	float4   m_sampling_resolution;
	float4   m_sampling_weight;
	float4   m_move_dir;
	float	 m_cloud_density;
	float	 m_vertical_fade;
	float	 m_horizontal_fade;
	float	 m_per_pixel_fade_threshhold;
	float    m_per_sample_fade_threshhold;
	float	 m_in_scattering_strength;
	float	 m_out_scattering_strength;
	float	 m_time;
	float    m_speed;
}

float rand1dTo1d(float3 value, float mutator = 0.546)
{

	float random = frac(sin(value + mutator) * 143758.5453);
	return random;
}

float3 rand1dTo3d(float value)
{
	return float3
		(
			rand1dTo1d(value, 3.9812),
			rand1dTo1d(value, 7.1536),
			rand1dTo1d(value, 5.7241)
			);
}

float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
	float2 smallValue = sin(value);
	float random = dot(smallValue, dotDir);
	random = frac(sin(random) * 143758.5453);
	return random;
}

float2 rand2dTo2d(float2 value) {
	return float2(
		rand2dTo1d(value, float2(12.989, 78.233)),
		rand2dTo1d(value, float2(39.346, 11.135))
		);
}

float rand3dTo1d(float3 value, float3 dotDir = float3(12.9898, 78.233, 37.719))
{
	//make value smaller to avoid artifacts
	float3 smallValue = sin(value);
	//get scalar value from 3d vector
	float random = dot(smallValue, dotDir);
	//make value more random by making it bigger and then taking the factional part
	random = frac(sin(random) * 43758.5453);
	return random;
}

float3 rand3dTo3d(float3 value)
{
	return float3(
		rand3dTo1d(value, float3(12.989, 78.233, 37.719)),
		rand3dTo1d(value, float3(39.346, 11.135, 83.155)),
		rand3dTo1d(value, float3(73.156, 52.235, 09.151))
		);
}

inline float easeIn(float interpolator)
{
	return interpolator * interpolator;
}

float easeOut(float interpolator)
{
	return 1 - easeIn(1 - interpolator);
}

float easeInOut(float interpolator)
{
	float easeInValue = easeIn(interpolator);
	float easeOutValue = easeOut(interpolator);
	return lerp(easeInValue, easeOutValue, interpolator);
}

float ValueNoise2d(float2 value)
{
	float upperLeftCell = rand2dTo1d(float2(floor(value.x), ceil(value.y)));
	float upperRightCell = rand2dTo1d(float2(ceil(value.x), ceil(value.y)));
	float lowerLeftCell = rand2dTo1d(float2(floor(value.x), floor(value.y)));
	float lowerRightCell = rand2dTo1d(float2(ceil(value.x), floor(value.y)));

	float interpolatorX = easeInOut(frac(value.x));
	float interpolatorY = easeInOut(frac(value.y));

	float upperCells = lerp(upperLeftCell, upperRightCell, interpolatorX);
	float lowerCells = lerp(lowerLeftCell, lowerRightCell, interpolatorX);

	float noise = lerp(lowerCells, upperCells, interpolatorY);
	return noise;
}

float3 ValueNoise3d(float3 value)
{
	float interpolatorX = easeInOut(frac(value.x));
	float interpolatorY = easeInOut(frac(value.y));
	float interpolatorZ = easeInOut(frac(value.z));

	float3 cellNoiseZ[2];
	[unroll]
	for (int z = 0; z <= 1; z++) {
		float3 cellNoiseY[2];
		[unroll]
		for (int y = 0; y <= 1; y++) {
			float3 cellNoiseX[2];
			[unroll]
			for (int x = 0; x <= 1; x++) {
				float3 cell = floor(value) + float3(x, y, z);
				cellNoiseX[x] = rand3dTo3d(cell);
			}
			cellNoiseY[y] = lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
		}
		cellNoiseZ[z] = lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
	}
	float3 noise = lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
	return noise;
}

float gradientNoise(float value)
{
	float fraction = frac(value);
	float interpolator = easeInOut(fraction);

	float previousCellInclination = rand1dTo1d(floor(value)) * 2 - 1;
	float previousCellLinePoint = previousCellInclination * fraction;

	float nextCellInclination = rand1dTo1d(ceil(value)) * 2 - 1;
	float nextCellLinePoint = nextCellInclination * (fraction - 1);

	return lerp(previousCellLinePoint, nextCellLinePoint, interpolator);
}

float perlinNoise(float2 value)
{
	float2 fraction = float2(frac(value.x), frac(value.y));

	float2 lowerLeftDirection = rand2dTo2d(float2(floor(value.x), floor(value.y))) * 2 - 1;
	float2 lowerRightDirection = rand2dTo2d(float2(ceil(value.x), floor(value.y))) * 2 - 1;
	float2 upperLeftDirection = rand2dTo2d(float2(floor(value.x), ceil(value.y))) * 2 - 1;
	float2 upperRightDirection = rand2dTo2d(float2(ceil(value.x), ceil(value.y))) * 2 - 1;

	float2 lowerLeftFunctionValue = dot(lowerLeftDirection, fraction - float2(0, 0));
	float2 lowerRightFunctionValue = dot(lowerRightDirection, fraction - float2(1, 0));
	float2 upperLeftFunctionValue = dot(upperLeftDirection, fraction - float2(0, 1));
	float2 upperRightFunctionValue = dot(upperRightDirection, fraction - float2(1, 1));

	float interpolatorX = easeInOut(fraction.x);
	float interpolatorY = easeInOut(fraction.y);


	float lowerCells = lerp(lowerLeftFunctionValue, lowerRightFunctionValue, interpolatorX);
	float upperCells = lerp(upperLeftFunctionValue, upperRightFunctionValue, interpolatorX);

	float noise = lerp(lowerCells, upperCells, interpolatorY);

	return noise;
}

float perlinNoise(float3 value)
{
	float3 fraction = frac(value);

	float interpolatorX = easeInOut(fraction.x);
	float interpolatorY = easeInOut(fraction.y);
	float interpolatorZ = easeInOut(fraction.z);

	float3 cellNoiseZ[2];
	[unroll]
	for (int z = 0; z <= 1; z++) {
		float3 cellNoiseY[2];
		[unroll]
		for (int y = 0; y <= 1; y++) {
			float3 cellNoiseX[2];
			[unroll]
			for (int x = 0; x <= 1; x++) {
				float3 cell = floor(value) + float3(x, y, z);
				float3 cellDirection = rand3dTo3d(cell) * 2 - 1;
				float3 compareVector = fraction - float3(x, y, z);
				cellNoiseX[x] = dot(cellDirection, compareVector);
			}
			cellNoiseY[y] = lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
		}
		cellNoiseZ[z] = lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
	}
	float3 noise = lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
	return noise;
}

float3 voronoiNoise(float3 value)
{
	float3 baseCell = floor(value);

	//first pass to find the closest cell
	float minDistToCell = 10;
	float3 toClosestCell;
	float3 closestCell;
	[unroll]
	for (int x1 = -1; x1 <= 1; x1++) {
		[unroll]
		for (int y1 = -1; y1 <= 1; y1++) {
			[unroll]
			for (int z1 = -1; z1 <= 1; z1++) {
				float3 cell = baseCell + float3(x1, y1, z1);
				float3 cellPosition = cell + rand3dTo3d(cell);
				float3 toCell = cellPosition - value;
				float distToCell = length(toCell);
				if (distToCell < minDistToCell) {
					minDistToCell = distToCell;
					closestCell = cell;
					toClosestCell = toCell;
				}
			}
		}
	}

	//second pass to find the distance to the closest edge
	float minEdgeDistance = 10;
	[unroll]
	for (int x2 = -1; x2 <= 1; x2++) {
		[unroll]
		for (int y2 = -1; y2 <= 1; y2++) {
			[unroll]
			for (int z2 = -1; z2 <= 1; z2++) {
				float3 cell = baseCell + float3(x2, y2, z2);
				float3 cellPosition = cell + rand3dTo3d(cell);
				float3 toCell = cellPosition - value;

				float3 diffToClosestCell = abs(closestCell - cell);
				bool isClosestCell = diffToClosestCell.x + diffToClosestCell.y + diffToClosestCell.z < 0.1;
				if (!isClosestCell) {
					float3 toCenter = (toClosestCell + toCell) * 0.5;
					float3 cellDifference = normalize(toCell - toClosestCell);
					float edgeDistance = dot(toCenter, cellDifference);
					minEdgeDistance = min(minEdgeDistance, edgeDistance);
				}
			}
		}
	}

	float random = rand3dTo1d(closestCell);
	return float3(minDistToCell, random, minEdgeDistance);
}

float3 modulo(float3 divident, float3 divisor) {
	float3 positiveDivident = divident % divisor + divisor;
	return positiveDivident % divisor;
}

float tiledPerlinNoise(float3 value, float period)
{
	float3 fraction = frac(value);

	float interpolatorX = easeInOut(fraction.x);
	float interpolatorY = easeInOut(fraction.y);
	float interpolatorZ = easeInOut(fraction.z);

	float3 cellNoiseZ[2];
	[unroll]
	for (int z = 0; z <= 1; z++) {
		float3 cellNoiseY[2];
		[unroll]
		for (int y = 0; y <= 1; y++) {
			float3 cellNoiseX[2];
			[unroll]
			for (int x = 0; x <= 1; x++) {
				float3 cell = (floor(value) + float3(x, y, z)) % period;
				float3 cellDirection = rand3dTo3d(cell) * 2 - 1;
				float3 compareVector = fraction - float3(x, y, z);
				cellNoiseX[x] = dot(cellDirection, compareVector);
			}
			cellNoiseY[y] = lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
		}
		cellNoiseZ[z] = lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
	}
	float3 noise = lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
	return noise;
}

float3 tiledVoronoiNoise(float3 value, float period)
{
	float3 baseCell = floor(value);

	//first pass to find the closest cell
	float minDistToCell = 10;
	float3 toClosestCell;
	float3 closestCell;
	[unroll]
	for (int x1 = -1; x1 <= 1; x1++) {
		[unroll]
		for (int y1 = -1; y1 <= 1; y1++) {
			[unroll]
			for (int z1 = -1; z1 <= 1; z1++) {
				float3 cell = baseCell + float3(x1, y1, z1);
				float3 tiledCell = modulo(cell, period);
				float3 cellPosition = cell + rand3dTo3d(tiledCell);
				float3 toCell = cellPosition - value;
				float distToCell = length(toCell);
				if (distToCell < minDistToCell) {
					minDistToCell = distToCell;
					closestCell = cell;
					toClosestCell = toCell;
				}
			}
		}
	}

	//second pass to find the distance to the closest edge
	float minEdgeDistance = 10;
	[unroll]
	for (int x2 = -1; x2 <= 1; x2++) {
		[unroll]
		for (int y2 = -1; y2 <= 1; y2++) {
			[unroll]
			for (int z2 = -1; z2 <= 1; z2++) {
				float3 cell = baseCell + float3(x2, y2, z2);
				float3 tiledCell = modulo(cell, period);
				float3 cellPosition = cell + rand3dTo3d(tiledCell);
				float3 toCell = cellPosition - value;

				float3 diffToClosestCell = abs(closestCell - cell);
				bool isClosestCell = diffToClosestCell.x + diffToClosestCell.y + diffToClosestCell.z < 0.1;
				if (!isClosestCell)
				{
					float3 toCenter = (toClosestCell + toCell) * 0.5;
					float3 cellDifference = normalize(toCell - toClosestCell);
					float edgeDistance = dot(toCenter, cellDifference);
					minEdgeDistance = min(minEdgeDistance, edgeDistance);
				}
			}
		}
	}

	float random = rand3dTo1d(closestCell);
	return float3(minDistToCell, random, minEdgeDistance);
}

float3 brownianPerlin(float3 value, int octaves, float gain, float lacunarity, float amplitude)
{
	float3 result = 0;

	for (int i = 0; i < octaves; i++)
	{
		result += perlinNoise(value) * amplitude;
		//frequency *= lacunarity;
		value *= lacunarity;
		amplitude *= gain;
	}

	return result;
}

float3 brownianTiledPerlin(float3 value, int octaves, int frequency, float gain, float lacunarity, float amplitude)
{
	float3 result = 0;

	for (int i = 0; i < octaves; i++)
	{
		result += tiledPerlinNoise(value, frequency) * amplitude;
		frequency *= lacunarity;
		value *= lacunarity;
		amplitude *= gain;
	}

	return result;
}

float3 brownianTiledVoronoi(float3 value, int octaves, int frequency, float gain, float lacunarity, float amplitude)
{
	float3 result = 0;

	for (int i = 0; i < octaves; i++)
	{
		result += tiledVoronoiNoise(value, frequency) * amplitude;
		frequency *= lacunarity;
		value *= lacunarity;
		amplitude *= gain;
	}

	return result;
}

float stratus_height = 2000.0;

//returns the xyz position of the cloud and the cloud concentration in w
float4 getStratus(float3 pos, float3 dir) : SV_TARGET
{
	//get the position in the cloud layers that the eye is looking at
	//float stratus_relative_height = m_camera_position.y + stratus_height;
	float steps_to_stratus = stratus_height / dir.y;
	float3 stratus_pos = dir * steps_to_stratus;

	float3 value = float3(stratus_pos.xz + m_time * m_speed * m_move_dir.xz, 0) * m_per_cell_size / 10;
	//float3 value2 = float3(pos.xy + float3(1000, 1000, 1000) + m_time * m_speed * m_move_dir.xz, 0) * m_per_cell_size;
	//float3 value3 = float3(pos.xy + float3(2000, 2000, 2000) + m_time * m_speed * m_move_dir.xz, 0) * m_per_cell_size;

	float noise = min(brownianPerlin(value, m_per_octaves, m_per_gain, m_per_lacunarity, m_per_amplitude), 1);
	//float noise2 = min(brownianPerlin(value2, m_per_octaves, m_per_gain, m_per_lacunarity, m_per_amplitude), 1);
	//float noise3 = min(brownianPerlin(value3, m_per_octaves, m_per_gain, m_per_lacunarity, m_per_amplitude), 1);

	//float3 final_noise = float3((noise - m_per_pixel_fade_threshhold) * (1 / (1 - m_per_pixel_fade_threshhold)),
	//	(noise2 - m_per_pixel_fade_threshhold) * (1 / (1 - m_per_pixel_fade_threshhold)),
	//	(noise3 - m_per_pixel_fade_threshhold) * (1 / (1 - m_per_pixel_fade_threshhold)));

	//return position of the cloud and the amount of cloud presence
	//return float4(stratus_pos, (noise - m_per_pixel_fade_threshhold) * (1 / (1 - m_per_pixel_fade_threshhold)));
	return float4(stratus_pos, (noise - m_per_pixel_fade_threshhold) * (1 / (1 - m_per_pixel_fade_threshhold)));
}

//===============================================================//
//   Cloud raymarching
//===============================================================//
////the fade mod returns a value from 1.0 (no fade) to 0 (fully faded) based on distance from the origin of the cloud
//float fadeMod(float3 pos, float3 obj_pos, float3 obj_size, float3 fade_dist)
//{
//	//check how far the point is from the core of the cloud
//	float3 relative_pos = pos - obj_pos;
//	//the cloud will only start to fade if the distance exceeds the vfade or wfade threshhold
//	float3 fade_weight = abs(relative_pos) - fade_dist;
//	fade_weight = fade_weight / (m_cloud_size.xyz / 2.0 - fade_dist);
//	fade_weight = 1.0 - fade_weight;
//
//
//	//combine the results (the Y value is being factored twice because it has more weight than the xz axis in defining cloud shape)
//	return min(fade_weight.x * fade_weight.y * fade_weight.y * fade_weight.z, 1);
//}

static const float base_num_steps = 10;
static const float step_size = 0.2f;

float raymarchCloud(float3 pos, float3 dir, float cloud_presence)
{
	float density = 0;
	dir = normalize(dir);

	float3 p = normalize(float3(pos.x, pos.y, pos.z)) * 10;
	//float3 p = 0;
	float sample_vp, sample_v_l, sample_v_m, sample_v_s, mix_sample;
	float concentration;

	//[unroll]
	int num_steps = floor(base_num_steps * cloud_presence);
	for (int i = 0; i < num_steps; i++)
	{
		//sample the cloud
		//sample_vp = Texture.SampleLevel(TextureSampler, (p + m_move_dir.xyz * m_time * m_speed) / m_sampling_resolution.x, 0).x
		//	* m_sampling_weight.x;
		//sample_v_l = Texture.SampleLevel(TextureSampler, (p + m_move_dir.xyz * m_time * m_speed) / m_sampling_resolution.y, 0).y
		//	* m_sampling_weight.y;
		//sample_v_m = Texture.SampleLevel(TextureSampler, (p + m_move_dir.xyz * m_time * m_speed) / m_sampling_resolution.z, 0).z
		//	* m_sampling_weight.z;
		//sample_v_s = Texture.SampleLevel(TextureSampler, (p + m_move_dir.xyz * m_time * m_speed) / m_sampling_resolution.w, 0).w
		//	* m_sampling_weight.w;

		//mix_sample = (sample_vp + sample_v_l + sample_v_m + sample_v_s);

		mix_sample = Texture.SampleLevel(TextureSampler, (p) / m_sampling_resolution.x, 0).x * m_sampling_weight.x;
		
		//mix_sample *= fadeMod(p, m_cloud_position.xyz, m_cloud_size.xyz, fade_dist);

		//mix_sample *= (mix_sample > m_per_sample_fade_threshhold);

		density += mix_sample;// *m_cloud_density;

		p += dir * step_size;
	}


	return density;// *cloud_presence;//(density * cloud_presence) * ((density * cloud_presence) > m_per_pixel_fade_threshhold);
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
	float4 cloud = 0;
	if (n.y > 0.1)
	{
		cloud = getStratus(input.world_pos, n);
		//if (cloud.w > 0)
		//{
			float dist = length(cloud.xyz);

			//cloud.xyz = cloud.xyz * (100 / dist);
			cloud_color = raymarchCloud(cloud.xyz, n, cloud.w);
			//cloud_color = cloud;

			//float cloud_transparency = min(((n.y - 0.1) / 0.3), 1);
			//cloud_color *= cloud_transparency;


		//}
	}

	//=================================================================


	//float3 final_color = final_sky_color * (1.0 - cloud_color.w) + cloud.w;
	//float3 final_color = cloud.w;
	//return float4(final_color, 1);
	return float4(1, 1, 1, cloud_color);
}
