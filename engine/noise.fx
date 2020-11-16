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