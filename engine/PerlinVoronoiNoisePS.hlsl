
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

float perlinNoise(float3 value) {
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

float3 modulo(float3 divident, float3 divisor) {
	float3 positiveDivident = divident % divisor + divisor;
	return positiveDivident % divisor;
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


static const float cell_sizeV = 1.5;
static const float period = 4;


static const float cell_sizeV2 = 0.75;
static const float cell_sizeV3 = 0.3525;
static const float cell_sizeP = 0.3525;
static const float cell_sizeP2 = 0.17625;


float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 valueVoronoi = input.world_pos.xyz / cell_sizeV;
	float3 valueVoronoi2= input.world_pos.xyz / cell_sizeV2;
	float3 valueVoronoi3 = input.world_pos.xyz / cell_sizeV3;
	float3 perlin = input.world_pos.xyz / cell_sizeP;
	float3 perlin2 = input.world_pos.xyz / cell_sizeP2;

	float blend1 = 0.3;
	float blend2 = 0.8;

	float v = easeInOut(tiledVoronoiNoise(valueVoronoi, period).x);
	float v2 = easeOut(tiledVoronoiNoise(valueVoronoi2, period).x);
	float v3 = easeOut(tiledVoronoiNoise(valueVoronoi3, period).x);
	//float v2 = 1 - easeOut(voronoiNoise(valueVoronoi).x);
	//float v3 = 1 - easeOut(voronoiNoise(valueVoronoi).x);
	float p = tiledPerlinNoise(perlin, period) + 0.5;
	float p2 = tiledPerlinNoise(perlin2, period) + 0.5;
	float finalP = p * 0.3 + p2 * 0.7;

	//float blended1 = v * (1.0f - blend1) + v * v2 * 1.25f * blend1;
	//float blended2 = blended1 * (1.0f - blend2) + blended1 * v3 * 1.25f * blend2;

	float blended1 = v3 * (1 - blend1) + v2 * blend1;
	float blended2 = blended1 * (1 - blend2) + v * blend2;
	blended2 = 1 - pow(blended2, 2);

	float final = min((1 - (v * 0.5 + v2 * 0.35 + v3 * 0.15)) * (1 - 0.064) + p2 * 0.064, 1);
	
	final = easeInOut(final);
	return float4(1,1,1, final);
}