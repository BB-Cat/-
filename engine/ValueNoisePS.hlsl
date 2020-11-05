
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
};

//mesh lighting characteristics 
cbuffer constant: register(b1)
{
	float	Ns; //specular power
	float	Ni; //optical density
	float	d;	//transparency 
	float	Tr;   //transparency 
	float	Tf; //transmission filter
	float3	Ka; //ambient color
	float3	Kd; //diffuse color
	float3	Ks; //specular color
	float	Ke; //emissive color
};


//float randShade(float3 p)
//{
//	float2 val = sin(p);
//	float random = dot(val, float2(12.9898, 78.233)); //float3(12.9898, 78.233, 37.719));
//
//	random = frac(random * 43758.5453);
//	return random;
//}

float rand1dTo1d(float3 value, float mutator = 0.546)
{
	float random = frac(sin(value + mutator) * 43758.5453);
	return random;
}

float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233)) {
	float2 smallValue = sin(value);
	float random = dot(smallValue, dotDir);
	random = frac(sin(random) * 143758.5453);
	return random;
}

float rand3dTo1d(float3 value, float3 dotDir = float3(12.9898, 78.233, 37.719)) {
	//make value smaller to avoid artifacts
	float3 smallValue = sin(value);
	//get scalar value from 3d vector
	float random = dot(smallValue, dotDir);
	//make value more random by making it bigger and then taking the factional part
	random = frac(sin(random) * 43758.5453);
	return random;
}

float3 rand3dTo3d(float3 value) {
	return float3(
		rand3dTo1d(value, float3(12.989, 78.233, 37.719)),
		rand3dTo1d(value, float3(39.346, 11.135, 83.155)),
		rand3dTo1d(value, float3(73.156, 52.235, 09.151))
		);
}

//float gradientNoise(float value) {
//	float previousCellNoise = rand1dTo1d(floor(value));
//	float nextCellNoise = rand1dTo1d(ceil(value));
//	float interpolator = frac(value);
//	interpolator = easeInOut(interpolator);
//	return lerp(previousCellNoise, nextCellNoise, interpolator);
//}



inline float easeIn(float interpolator) 
{
	return interpolator * interpolator;
}

float easeOut(float interpolator) {
	return 1 - easeIn(1 - interpolator);
}

float easeInOut(float interpolator) {
	float easeInValue = easeIn(interpolator);
	float easeOutValue = easeOut(interpolator);
	return lerp(easeInValue, easeOutValue, interpolator);
}

float ValueNoise2d(float2 value) {
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

float3 ValueNoise3d(float3 value) {
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



static const float cell_size = 0.2;

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 value = input.world_pos.xyz / cell_size;
	float3 noise = ValueNoise3d(value);

	return float4(noise, 1);
}