#pragma once
#include "Vector3D.h"
#include "Vector2D.h"
#include <math.h>
//#include <cmath>


#define DEFAULT_MUTATION (63758.5453f)

class NoiseGenerator
{
public:

	void setSeed(float seed)
	{
		m_seed = DEFAULT_MUTATION - frac(seed / 8.745f * seed / 8.745f) * DEFAULT_MUTATION / 2.0f;
	}

	float ValueNoise2d(Vec2 value)
	{
		float upperLeftCell = rand2dTo1d(Vec2(floor(value.x), ceil(value.y)));
		float upperRightCell = rand2dTo1d(Vec2(ceil(value.x), ceil(value.y)));
		float lowerLeftCell = rand2dTo1d(Vec2(floor(value.x), floor(value.y)));
		float lowerRightCell = rand2dTo1d(Vec2(ceil(value.x), floor(value.y)));

		float interpolatorX = easeInOut(frac(value.x));
		float interpolatorY = easeInOut(frac(value.y));

		float upperCells = lerp(upperLeftCell, upperRightCell, interpolatorX);
		float lowerCells = lerp(lowerLeftCell, lowerRightCell, interpolatorX);

		float noise = lerp(lowerCells, upperCells, interpolatorY);
		return noise;
	}

	Vec3 ValueNoise3d(Vec3 value)
	{
		float interpolatorX = easeInOut(frac(value.x));
		float interpolatorY = easeInOut(frac(value.y));
		float interpolatorZ = easeInOut(frac(value.z));

		Vec3 cellNoiseZ[2];
		//[unroll]
		for (int z = 0; z <= 1; z++) {
			Vec3 cellNoiseY[2];
			//[unroll]
			for (int y = 0; y <= 1; y++) {
				Vec3 cellNoiseX[2];
				//[unroll]
				for (int x = 0; x <= 1; x++) {
					Vec3 cell = floor(value) + Vec3(x, y, z);
					cellNoiseX[x] = rand3dTo3d(cell);
				}
				cellNoiseY[y] = Vec3::lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
			}
			cellNoiseZ[z] = Vec3::lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
		}
		Vec3 noise = Vec3::lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
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

	float perlinNoise(Vec2 value)
	{
		Vec2 fraction = Vec2(frac(value.x), frac(value.y));

		Vec2 lowerLeftDirection = rand2dTo2d(Vec2(floor(value.x), floor(value.y))) * 2 - 1;
		Vec2 lowerRightDirection = rand2dTo2d(Vec2(ceil(value.x), floor(value.y))) * 2 - 1;
		Vec2 upperLeftDirection = rand2dTo2d(Vec2(floor(value.x), ceil(value.y))) * 2 - 1;
		Vec2 upperRightDirection = rand2dTo2d(Vec2(ceil(value.x), ceil(value.y))) * 2 - 1;

		float lowerLeftFunctionValue = Vec2::dot(lowerLeftDirection, fraction - Vec2(0, 0));
		float lowerRightFunctionValue = Vec2::dot(lowerRightDirection, fraction - Vec2(1, 0));
		float upperLeftFunctionValue = Vec2::dot(upperLeftDirection, fraction - Vec2(0, 1));
		float upperRightFunctionValue = Vec2::dot(upperRightDirection, fraction - Vec2(1, 1));

		float interpolatorX = easeInOut(fraction.x);
		float interpolatorY = easeInOut(fraction.y);


		float lowerCells = lerp(lowerLeftFunctionValue, lowerRightFunctionValue, interpolatorX);
		float upperCells = lerp(upperLeftFunctionValue, upperRightFunctionValue, interpolatorX);

		float noise = lerp(lowerCells, upperCells, interpolatorY);

		return noise;
	}

	float perlinNoise(Vec3 value) {
		Vec3 fraction = frac(value);

		float interpolatorX = easeInOut(fraction.x);
		float interpolatorY = easeInOut(fraction.y);
		float interpolatorZ = easeInOut(fraction.z);

		float cellNoiseZ[2];
		//[unroll]
		for (int z = 0; z <= 1; z++) {
			float cellNoiseY[2];
			//[unroll]
			for (int y = 0; y <= 1; y++) {
				float cellNoiseX[2];
				//[unroll]
				for (int x = 0; x <= 1; x++) {
					Vec3 cell = floor(value) + Vec3(x, y, z);
					Vec3 cellDirection = rand3dTo3d(cell) * 2 - 1;
					Vec3 compareVector = fraction - Vec3(x, y, z);
					cellNoiseX[x] = Vec3::dot(cellDirection, compareVector);
				}
				cellNoiseY[y] = lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
			}
			cellNoiseZ[z] = lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
		}
		float noise = lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
		return noise;
	}

	Vec3 voronoiNoise(Vec3 value)
	{
		Vec3 baseCell = floor(value);

		//first pass to find the closest cell
		float minDistToCell = 10;
		Vec3 toClosestCell;
		Vec3 closestCell;
		//[unroll]
		for (int x1 = -1; x1 <= 1; x1++) 
		{
			//[unroll]
			for (int y1 = -1; y1 <= 1; y1++) 
			{
				//[unroll]
				for (int z1 = -1; z1 <= 1; z1++) 
				{
					Vec3 cell = baseCell + Vec3(x1, y1, z1);
					Vec3 cellPosition = cell + rand3dTo3d(cell);
					Vec3 toCell = cellPosition - value;
					float distToCell = toCell.length();
					if (distToCell < minDistToCell) 
					{
						minDistToCell = distToCell;
						closestCell = cell;
						toClosestCell = toCell;
					}
				}
			}
		}

		//second pass to find the distance to the closest edge
		float minEdgeDistance = 10;
		//[unroll]
		for (int x2 = -1; x2 <= 1; x2++) {
			//[unroll]
			for (int y2 = -1; y2 <= 1; y2++) {
				//[unroll]
				for (int z2 = -1; z2 <= 1; z2++) {
					Vec3 cell = baseCell + Vec3(x2, y2, z2);
					Vec3 cellPosition = cell + rand3dTo3d(cell);
					Vec3 toCell = cellPosition - value;

					Vec3 diffToClosestCell = abs(closestCell - cell);
					bool isClosestCell = diffToClosestCell.x + diffToClosestCell.y + diffToClosestCell.z < 0.1;
					if (!isClosestCell) {
						Vec3 toCenter = (toClosestCell + toCell) * 0.5;
						Vec3 cellDifference = (toCell - toClosestCell);
						cellDifference.normalize();
						float edgeDistance = Vec3::dot(toCenter, cellDifference);
						minEdgeDistance = min(minEdgeDistance, edgeDistance);
					}
				}
			}
		}

		float random = rand3dTo1d(closestCell);
		return Vec3(minDistToCell, random, minEdgeDistance);
	}

	float tiledPerlinNoise(Vec3 value, float period)
	{
		Vec3 fraction = frac(value);

		float interpolatorX = easeInOut(fraction.x);
		float interpolatorY = easeInOut(fraction.y);
		float interpolatorZ = easeInOut(fraction.z);

		float cellNoiseZ[2];
		//[unroll]
		for (int z = 0; z <= 1; z++) {
			float cellNoiseY[2];
			//[unroll]
			for (int y = 0; y <= 1; y++) {
				float cellNoiseX[2];
				//[unroll]
				for (int x = 0; x <= 1; x++) {
					Vec3 cell = modulo(floor(value) + Vec3(x, y, z), period);// % period;
					Vec3 cellDirection = rand3dTo3d(cell) * 2 - 1;
					Vec3 compareVector = fraction - Vec3(x, y, z);
					cellNoiseX[x] = Vec3::dot(cellDirection, compareVector);
				}
				cellNoiseY[y] = lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
			}
			cellNoiseZ[z] = lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
		}
		float noise = lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
		return noise;
	}

	Vec2 modulo(Vec2 divident, int divisor)
	{
		int x = ((int)divident.x + divisor * 100) % (divisor);
		int y = ((int)divident.y + divisor * 100) % (divisor);

		return Vec2(x, y);
	}

	Vec3 modulo(Vec3 divident, int divisor) 
	{
		//float divX = divident.m_x / divisor;
		//float divY = divident.m_y / divisor;
		//float divZ = divident.m_z / divisor;

		//float x = (int)(divX * divisor + divisor) % (int)(divisor);
		//float y = (int)(divY * divisor + divisor) % (int)(divisor);
		//float z = (int)(divZ * divisor + divisor) % (int)(divisor);


		int x = ((int)divident.x + divisor * 100) % (divisor);
		int y = ((int)divident.y + divisor * 100) % (divisor);
		int z = ((int)divident.z + divisor * 100) % (divisor);

		//float3 positiveDivident = divident % divisor + divisor;
		//return positiveDivident % divisor;



		return Vec3(x, y, z);
	}

	Vec3 tiledVoronoiNoise2D(Vec2 value, float period) 
	{
		Vec2 baseCell = floor(value);

		//first pass to find the closest cell
		float minDistToCell = 100;
		Vec2 toClosestCell;
		Vec2 closestCell;

		for (int x1 = -1; x1 <= 1; x1++) 
		{

			for (int y1 = -1; y1 <= 1; y1++) 
			{
				Vec2 cell = baseCell + Vec2(x1, y1);
				Vec2 cellPosition = cell + rand2dTo2d(cell);
				Vec2 toCell = cellPosition - value;
				float distToCell = toCell.length();
				if (distToCell < minDistToCell) 
				{
					minDistToCell = distToCell;
					closestCell = cell;
					toClosestCell = toCell;
				}
				//Vector2D cell = baseCell + Vector2D(x1, y1);
				//Vector2D tiledCell = modulo(cell, period);
				//Vector2D cellPosition = cell + rand2dTo2d(tiledCell);
				//Vector2D toCell = cellPosition - value;
				//float distToCell = toCell.length();
				//if (distToCell < minDistToCell)
				//{
				//	minDistToCell = distToCell;
				//	closestCell = cell;
				//	toClosestCell = toCell;
				//}
			}
		}

		//second pass to find the distance to the closest edge
		float minEdgeDistance = 100;

		for (int x2 = -1; x2 <= 1; x2++)
		{

			for (int y2 = -1; y2 <= 1; y2++) 
			{
				Vec2 cell = baseCell + Vec2(x2, y2);
				Vec2 cellPosition = cell + rand2dTo2d(cell);
				Vec2 toCell = cellPosition - value;

				Vec2 diffToClosestCell = abs(closestCell - cell);
				bool isClosestCell = diffToClosestCell.x + diffToClosestCell.y < 0.1;
				if (!isClosestCell) 
				{
					Vec2 toCenter = (toClosestCell + toCell) * 0.5;
					Vec2 cellDifference = (toCell - toClosestCell);
					cellDifference.normalize();
					float edgeDistance = Vec2::dot(toCenter, cellDifference);
					minEdgeDistance = min(minEdgeDistance, edgeDistance);
				}

				//Vector2D cell = baseCell + Vector2D(x2, y2);
				//Vector2D tiledCell = modulo(cell, period);
				//Vector2D cellPosition = cell + rand2dTo2d(tiledCell);
				//Vector2D toCell = cellPosition - value;

				//Vector2D diffToClosestCell = abs(closestCell - cell);
				//bool isClosestCell = diffToClosestCell.m_x + diffToClosestCell.m_y < 0.1;
				//if (!isClosestCell) 
				//{
				//	Vector2D toCenter = (toClosestCell + toCell) * 0.5;
				//	Vector2D cellDifference = (toCell - toClosestCell);
				//	cellDifference.normalize();
				//	float edgeDistance = Vector2D::dot(toCenter, cellDifference);
				//	minEdgeDistance = min(minEdgeDistance, edgeDistance);
				//}
			}
		}

		float random = rand2dTo1d(closestCell);
		return Vec3(minDistToCell, random, minEdgeDistance);
	}

	Vec3 tiledVoronoiNoise3D(Vec3 value, float period)
	{
		Vec3 baseCell = floor(value);

		//first pass to find the closest cell
		float minDistToCell = 10;
		Vec3 toClosestCell;
		Vec3 closestCell;

		for (int x1 = -1; x1 <= 1; x1++) {

			for (int y1 = -1; y1 <= 1; y1++) {

				for (int z1 = -1; z1 <= 1; z1++) 
				{
					Vec3 cell = baseCell + Vec3(x1, y1, z1);
					Vec3 tiledCell = modulo(cell, period);
					Vec3 cellPosition = cell + rand3dTo3d(tiledCell);
					Vec3 toCell = cellPosition - value;
					float distToCell = toCell.length();
					if (distToCell < minDistToCell) 
					{
						minDistToCell = distToCell;
						closestCell = cell;
						toClosestCell = toCell;
					}
				}
			}
		}

		//second pass to find the distance to the closest edge
		float minEdgeDistance = 10;

		for (int x2 = -1; x2 <= 1; x2++) {

			for (int y2 = -1; y2 <= 1; y2++) {

				for (int z2 = -1; z2 <= 1; z2++) {
					Vec3 cell = baseCell + Vec3(x2, y2, z2);
					Vec3 tiledCell = modulo(cell, period);
					Vec3 cellPosition = cell + rand3dTo3d(tiledCell);
					Vec3 toCell = cellPosition - value;

					Vec3 diffToClosestCell = abs(closestCell - cell);
					bool isClosestCell = diffToClosestCell.x + diffToClosestCell.y + diffToClosestCell.z < 0.1;
					if (!isClosestCell) {
						Vec3 toCenter = (toClosestCell + toCell) * 0.5;
						Vec3 cellDifference = (toCell - toClosestCell);
						cellDifference.normalize();
						float edgeDistance = Vec3::dot(toCenter, cellDifference);
						minEdgeDistance = min(minEdgeDistance, edgeDistance);
					}
				}
			}
		}

		float random = rand3dTo1d(closestCell);
		return Vec3(minDistToCell, random, minEdgeDistance);

	}

	float perlinVoronoiNoise(Vec3 value, float period)
	{
		static const float cell_sizeV = 2.0;
		static const float cell_sizeV2 = 1.0;
		static const float cell_sizeV3 = 0.5;
		static const float cell_sizeP = 1.0;
		static const float cell_sizeP2 = 0.5;


		Vec3 valueVoronoi = value / cell_sizeV;
		Vec3 valueVoronoi2 = value / cell_sizeV2;
		Vec3 valueVoronoi3 = value / cell_sizeV3;
		Vec3 perlin = value / cell_sizeP;
		Vec3 perlin2 = value / cell_sizeP2;

		float blend1 = 0.3;
		float blend2 = 0.8;
																	
		float v = easeInOut(tiledVoronoiNoise3D(valueVoronoi, period).x);
		float v2 = easeOut(tiledVoronoiNoise3D(valueVoronoi2, period).x);
		float v3 = easeOut(tiledVoronoiNoise3D(valueVoronoi3, period).x);


		float p = tiledPerlinNoise(perlin, period) + 0.5;
		float p2 = tiledPerlinNoise(perlin2, period) + 0.5;
		float finalP = p * 0.3 + p2 * 0.7;

		float blended1 = v3 * (1 - blend1) + v2 * blend1;
		float blended2 = blended1 * (1 - blend2) + v * blend2;
		blended2 = 1 - pow(blended2, 2);

		float final = min((1 - (v * 0.5 + v2 * 0.35 + v3 * 0.15)) * (1 - 0.064) + p2 * 0.064, 1);

		return easeInOut(final);
	}


	/*
	octaves - the amount of noise samples added to the result
	lacunarity - how much the frequency increases with each octave
	gain - how much the impact of each octave increases or decreases
	amplitude - multiplier to the final result
	frequency - how often the pattern loops
	*/
	Vec3 brownianTiledVoronoi(Vec3 value, int octaves, int frequency, float gain = 0.5f, float lacunarity = 2.0f, float amplitude = 1.0f)
	{
		Vec3 result = {};

		for (int i = 0; i < octaves; i++)
		{
			result += tiledVoronoiNoise3D(value, frequency) * amplitude;
			frequency *= lacunarity;
			value = value * lacunarity;
			amplitude *= gain;
		}

		return result;
	}

	Vec3 brownianTiledPerlin(Vec3 value, int octaves, int frequency, float gain = 0.5f, float lacunarity = 2.0f, float amplitude = 1.0f)
	{
		Vec3 result = {};

		for (int i = 0; i < octaves; i++)
		{
			result += tiledPerlinNoise(value, frequency) * amplitude;
			frequency *= lacunarity;
			value = value * lacunarity;
			amplitude *= gain;
		}

		return result;
	}

private:
	float abs(float num)
	{
		if (num >= 0) return num;
		else return (num * -1);
	}

	Vec2 abs(Vec2 num)
	{
		return Vec2(abs(num.x), abs(num.y));
	}

	Vec3 abs(Vec3 num)
	{
		return Vec3(abs(num.x), abs(num.y), abs(num.z));
	}

	float frac(float num)
	{
		float f = (num - (int)(num));
		if (f < 0) f *= -1;

		return f;
	}

	Vec3 frac(Vec3 v)
	{
		return Vec3((v.x - (int)(v.x)), (v.y - (int)(v.y)), (v.z - (int)(v.z)));
	}

	float lerp(float v1, float v2, float t)
	{
		return v1 * (1.0f - t) + v2 * t;
	}

	int floor(float num)
	{
		return num;
	}

	Vec2 floor(Vec2 num)
	{
		return Vec2((int)(num.x), (int)(num.y));
	}

	Vec3 floor(Vec3 num)
	{
		return Vec3((int)(num.x), (int)(num.y), (int)(num.z));
	}

	int ceil(float num)
	{
		return num + 1;
	}

	Vec2 ceil(Vec2 num)
	{
		return Vec2((int)(num.x + 1), (int)(num.y + 1));
	}

	Vec3 ceil(Vec3 num)
	{
		return Vec3((int)(num.x + 1), (int)(num.y + 1), (int)(num.z + 1));
	}

	float rand1dTo1d(float value, float mutator = 0.546)
	{

		float random = frac(sin(value + mutator) * m_seed);
		return random;
	}

	Vec3 rand1dTo3d(float value)
	{
		return Vec3
		(
			rand1dTo1d(value, 3.9812),
			rand1dTo1d(value, 7.1536),
			rand1dTo1d(value, 5.7241)
		);
	}

	float rand2dTo1d(Vec2 value, Vec2 dotDir = Vec2(12.9898, 78.233))
	{
		Vec2 smallValue(sin(value.x), sin(value.y));
		float random = Vec2::dot(smallValue, dotDir);
		random = frac(sin(random) * m_seed);
		return random;
	}

	Vec2 rand2dTo2d(Vec2 value) {
		return Vec2(
			rand2dTo1d(value, Vec2(12.989, 78.233)),
			rand2dTo1d(value, Vec2(39.346, 11.135))
		);
	}

	float rand3dTo1d(Vec3 value, Vec3 dotDir = Vec3(12.9898, 78.233, 37.719))
	{
		//make value smaller to avoid artifacts
		Vec3 smallValue(sin(value.x), sin(value.y), sin(value.z));
		//get scalar value from 3d vector
		float random = Vec3::dot(smallValue, dotDir);
		//make value more random by making it bigger and then taking the factional part
		random = frac(sin(random) * m_seed);
		return random;
	}

	Vec3 rand3dTo3d(Vec3 value)
	{
		return Vec3(
			rand3dTo1d(value, Vec3(12.989, 78.233, 37.719)),
			rand3dTo1d(value, Vec3(39.346, 11.135, 83.155)),
			rand3dTo1d(value, Vec3(73.156, 52.235, 09.151))
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

	private:
		float m_seed = DEFAULT_MUTATION;
};
