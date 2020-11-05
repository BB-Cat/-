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

	float ValueNoise2d(Vector2D value)
	{
		float upperLeftCell = rand2dTo1d(Vector2D(floor(value.m_x), ceil(value.m_y)));
		float upperRightCell = rand2dTo1d(Vector2D(ceil(value.m_x), ceil(value.m_y)));
		float lowerLeftCell = rand2dTo1d(Vector2D(floor(value.m_x), floor(value.m_y)));
		float lowerRightCell = rand2dTo1d(Vector2D(ceil(value.m_x), floor(value.m_y)));

		float interpolatorX = easeInOut(frac(value.m_x));
		float interpolatorY = easeInOut(frac(value.m_y));

		float upperCells = lerp(upperLeftCell, upperRightCell, interpolatorX);
		float lowerCells = lerp(lowerLeftCell, lowerRightCell, interpolatorX);

		float noise = lerp(lowerCells, upperCells, interpolatorY);
		return noise;
	}

	Vector3D ValueNoise3d(Vector3D value)
	{
		float interpolatorX = easeInOut(frac(value.m_x));
		float interpolatorY = easeInOut(frac(value.m_y));
		float interpolatorZ = easeInOut(frac(value.m_z));

		Vector3D cellNoiseZ[2];
		//[unroll]
		for (int z = 0; z <= 1; z++) {
			Vector3D cellNoiseY[2];
			//[unroll]
			for (int y = 0; y <= 1; y++) {
				Vector3D cellNoiseX[2];
				//[unroll]
				for (int x = 0; x <= 1; x++) {
					Vector3D cell = floor(value) + Vector3D(x, y, z);
					cellNoiseX[x] = rand3dTo3d(cell);
				}
				cellNoiseY[y] = Vector3D::lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
			}
			cellNoiseZ[z] = Vector3D::lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
		}
		Vector3D noise = Vector3D::lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
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

	float perlinNoise(Vector2D value)
	{
		Vector2D fraction = Vector2D(frac(value.m_x), frac(value.m_y));

		Vector2D lowerLeftDirection = rand2dTo2d(Vector2D(floor(value.m_x), floor(value.m_y))) * 2 - 1;
		Vector2D lowerRightDirection = rand2dTo2d(Vector2D(ceil(value.m_x), floor(value.m_y))) * 2 - 1;
		Vector2D upperLeftDirection = rand2dTo2d(Vector2D(floor(value.m_x), ceil(value.m_y))) * 2 - 1;
		Vector2D upperRightDirection = rand2dTo2d(Vector2D(ceil(value.m_x), ceil(value.m_y))) * 2 - 1;

		float lowerLeftFunctionValue = Vector2D::dot(lowerLeftDirection, fraction - Vector2D(0, 0));
		float lowerRightFunctionValue = Vector2D::dot(lowerRightDirection, fraction - Vector2D(1, 0));
		float upperLeftFunctionValue = Vector2D::dot(upperLeftDirection, fraction - Vector2D(0, 1));
		float upperRightFunctionValue = Vector2D::dot(upperRightDirection, fraction - Vector2D(1, 1));

		float interpolatorX = easeInOut(fraction.m_x);
		float interpolatorY = easeInOut(fraction.m_y);


		float lowerCells = lerp(lowerLeftFunctionValue, lowerRightFunctionValue, interpolatorX);
		float upperCells = lerp(upperLeftFunctionValue, upperRightFunctionValue, interpolatorX);

		float noise = lerp(lowerCells, upperCells, interpolatorY);

		return noise;
	}

	float perlinNoise(Vector3D value) {
		Vector3D fraction = frac(value);

		float interpolatorX = easeInOut(fraction.m_x);
		float interpolatorY = easeInOut(fraction.m_y);
		float interpolatorZ = easeInOut(fraction.m_z);

		float cellNoiseZ[2];
		//[unroll]
		for (int z = 0; z <= 1; z++) {
			float cellNoiseY[2];
			//[unroll]
			for (int y = 0; y <= 1; y++) {
				float cellNoiseX[2];
				//[unroll]
				for (int x = 0; x <= 1; x++) {
					Vector3D cell = floor(value) + Vector3D(x, y, z);
					Vector3D cellDirection = rand3dTo3d(cell) * 2 - 1;
					Vector3D compareVector = fraction - Vector3D(x, y, z);
					cellNoiseX[x] = Vector3D::dot(cellDirection, compareVector);
				}
				cellNoiseY[y] = lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
			}
			cellNoiseZ[z] = lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
		}
		float noise = lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
		return noise;
	}

	Vector3D voronoiNoise(Vector3D value)
	{
		Vector3D baseCell = floor(value);

		//first pass to find the closest cell
		float minDistToCell = 10;
		Vector3D toClosestCell;
		Vector3D closestCell;
		//[unroll]
		for (int x1 = -1; x1 <= 1; x1++) 
		{
			//[unroll]
			for (int y1 = -1; y1 <= 1; y1++) 
			{
				//[unroll]
				for (int z1 = -1; z1 <= 1; z1++) 
				{
					Vector3D cell = baseCell + Vector3D(x1, y1, z1);
					Vector3D cellPosition = cell + rand3dTo3d(cell);
					Vector3D toCell = cellPosition - value;
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
					Vector3D cell = baseCell + Vector3D(x2, y2, z2);
					Vector3D cellPosition = cell + rand3dTo3d(cell);
					Vector3D toCell = cellPosition - value;

					Vector3D diffToClosestCell = abs(closestCell - cell);
					bool isClosestCell = diffToClosestCell.m_x + diffToClosestCell.m_y + diffToClosestCell.m_z < 0.1;
					if (!isClosestCell) {
						Vector3D toCenter = (toClosestCell + toCell) * 0.5;
						Vector3D cellDifference = (toCell - toClosestCell);
						cellDifference.normalize();
						float edgeDistance = Vector3D::dot(toCenter, cellDifference);
						minEdgeDistance = min(minEdgeDistance, edgeDistance);
					}
				}
			}
		}

		float random = rand3dTo1d(closestCell);
		return Vector3D(minDistToCell, random, minEdgeDistance);
	}

	float tiledPerlinNoise(Vector3D value, float period)
	{
		Vector3D fraction = frac(value);

		float interpolatorX = easeInOut(fraction.m_x);
		float interpolatorY = easeInOut(fraction.m_y);
		float interpolatorZ = easeInOut(fraction.m_z);

		float cellNoiseZ[2];
		//[unroll]
		for (int z = 0; z <= 1; z++) {
			float cellNoiseY[2];
			//[unroll]
			for (int y = 0; y <= 1; y++) {
				float cellNoiseX[2];
				//[unroll]
				for (int x = 0; x <= 1; x++) {
					Vector3D cell = modulo(floor(value) + Vector3D(x, y, z), period);// % period;
					Vector3D cellDirection = rand3dTo3d(cell) * 2 - 1;
					Vector3D compareVector = fraction - Vector3D(x, y, z);
					cellNoiseX[x] = Vector3D::dot(cellDirection, compareVector);
				}
				cellNoiseY[y] = lerp(cellNoiseX[0], cellNoiseX[1], interpolatorX);
			}
			cellNoiseZ[z] = lerp(cellNoiseY[0], cellNoiseY[1], interpolatorY);
		}
		float noise = lerp(cellNoiseZ[0], cellNoiseZ[1], interpolatorZ);
		return noise;
	}

	Vector2D modulo(Vector2D divident, int divisor)
	{
		int x = ((int)divident.m_x + divisor * 100) % (divisor);
		int y = ((int)divident.m_y + divisor * 100) % (divisor);

		return Vector2D(x, y);
	}

	Vector3D modulo(Vector3D divident, int divisor) 
	{
		//float divX = divident.m_x / divisor;
		//float divY = divident.m_y / divisor;
		//float divZ = divident.m_z / divisor;

		//float x = (int)(divX * divisor + divisor) % (int)(divisor);
		//float y = (int)(divY * divisor + divisor) % (int)(divisor);
		//float z = (int)(divZ * divisor + divisor) % (int)(divisor);


		int x = ((int)divident.m_x + divisor * 100) % (divisor);
		int y = ((int)divident.m_y + divisor * 100) % (divisor);
		int z = ((int)divident.m_z + divisor * 100) % (divisor);

		//float3 positiveDivident = divident % divisor + divisor;
		//return positiveDivident % divisor;



		return Vector3D(x, y, z);
	}

	Vector3D tiledVoronoiNoise2D(Vector2D value, float period) 
	{
		Vector2D baseCell = floor(value);

		//first pass to find the closest cell
		float minDistToCell = 100;
		Vector2D toClosestCell;
		Vector2D closestCell;

		for (int x1 = -1; x1 <= 1; x1++) 
		{

			for (int y1 = -1; y1 <= 1; y1++) 
			{
				Vector2D cell = baseCell + Vector2D(x1, y1);
				Vector2D cellPosition = cell + rand2dTo2d(cell);
				Vector2D toCell = cellPosition - value;
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
				Vector2D cell = baseCell + Vector2D(x2, y2);
				Vector2D cellPosition = cell + rand2dTo2d(cell);
				Vector2D toCell = cellPosition - value;

				Vector2D diffToClosestCell = abs(closestCell - cell);
				bool isClosestCell = diffToClosestCell.m_x + diffToClosestCell.m_y < 0.1;
				if (!isClosestCell) 
				{
					Vector2D toCenter = (toClosestCell + toCell) * 0.5;
					Vector2D cellDifference = (toCell - toClosestCell);
					cellDifference.normalize();
					float edgeDistance = Vector2D::dot(toCenter, cellDifference);
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
		return Vector3D(minDistToCell, random, minEdgeDistance);
	}

	Vector3D tiledVoronoiNoise3D(Vector3D value, float period)
	{
		Vector3D baseCell = floor(value);

		//first pass to find the closest cell
		float minDistToCell = 10;
		Vector3D toClosestCell;
		Vector3D closestCell;

		for (int x1 = -1; x1 <= 1; x1++) {

			for (int y1 = -1; y1 <= 1; y1++) {

				for (int z1 = -1; z1 <= 1; z1++) 
				{
					Vector3D cell = baseCell + Vector3D(x1, y1, z1);
					Vector3D tiledCell = modulo(cell, period);
					Vector3D cellPosition = cell + rand3dTo3d(tiledCell);
					Vector3D toCell = cellPosition - value;
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
					Vector3D cell = baseCell + Vector3D(x2, y2, z2);
					Vector3D tiledCell = modulo(cell, period);
					Vector3D cellPosition = cell + rand3dTo3d(tiledCell);
					Vector3D toCell = cellPosition - value;

					Vector3D diffToClosestCell = abs(closestCell - cell);
					bool isClosestCell = diffToClosestCell.m_x + diffToClosestCell.m_y + diffToClosestCell.m_z < 0.1;
					if (!isClosestCell) {
						Vector3D toCenter = (toClosestCell + toCell) * 0.5;
						Vector3D cellDifference = (toCell - toClosestCell);
						cellDifference.normalize();
						float edgeDistance = Vector3D::dot(toCenter, cellDifference);
						minEdgeDistance = min(minEdgeDistance, edgeDistance);
					}
				}
			}
		}

		float random = rand3dTo1d(closestCell);
		return Vector3D(minDistToCell, random, minEdgeDistance);

	}

	float perlinVoronoiNoise(Vector3D value, float period)
	{
		static const float cell_sizeV = 2.0;
		static const float cell_sizeV2 = 1.0;
		static const float cell_sizeV3 = 0.5;
		static const float cell_sizeP = 1.0;
		static const float cell_sizeP2 = 0.5;


		Vector3D valueVoronoi = value / cell_sizeV;
		Vector3D valueVoronoi2 = value / cell_sizeV2;
		Vector3D valueVoronoi3 = value / cell_sizeV3;
		Vector3D perlin = value / cell_sizeP;
		Vector3D perlin2 = value / cell_sizeP2;

		float blend1 = 0.3;
		float blend2 = 0.8;
																	
		float v = easeInOut(tiledVoronoiNoise3D(valueVoronoi, period).m_x);
		float v2 = easeOut(tiledVoronoiNoise3D(valueVoronoi2, period).m_x);
		float v3 = easeOut(tiledVoronoiNoise3D(valueVoronoi3, period).m_x);


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
	Vector3D brownianTiledVoronoi(Vector3D value, int octaves, int frequency, float gain = 0.5f, float lacunarity = 2.0f, float amplitude = 1.0f)
	{
		Vector3D result = {};

		for (int i = 0; i < octaves; i++)
		{
			result += tiledVoronoiNoise3D(value, frequency) * amplitude;
			frequency *= lacunarity;
			value = value * lacunarity;
			amplitude *= gain;
		}

		return result;
	}

	Vector3D brownianTiledPerlin(Vector3D value, int octaves, int frequency, float gain = 0.5f, float lacunarity = 2.0f, float amplitude = 1.0f)
	{
		Vector3D result = {};

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

	Vector2D abs(Vector2D num)
	{
		return Vector2D(abs(num.m_x), abs(num.m_y));
	}

	Vector3D abs(Vector3D num)
	{
		return Vector3D(abs(num.m_x), abs(num.m_y), abs(num.m_z));
	}

	float frac(float num)
	{
		float f = (num - (int)(num));
		if (f < 0) f *= -1;

		return f;
	}

	Vector3D frac(Vector3D v)
	{
		return Vector3D((v.m_x - (int)(v.m_x)), (v.m_y - (int)(v.m_y)), (v.m_z - (int)(v.m_z)));
	}

	float lerp(float v1, float v2, float t)
	{
		return v1 * (1.0f - t) + v2 * t;
	}

	int floor(float num)
	{
		return num;
	}

	Vector2D floor(Vector2D num)
	{
		return Vector2D((int)(num.m_x), (int)(num.m_y));
	}

	Vector3D floor(Vector3D num)
	{
		return Vector3D((int)(num.m_x), (int)(num.m_y), (int)(num.m_z));
	}

	int ceil(float num)
	{
		return num + 1;
	}

	Vector2D ceil(Vector2D num)
	{
		return Vector2D((int)(num.m_x + 1), (int)(num.m_y + 1));
	}

	Vector3D ceil(Vector3D num)
	{
		return Vector3D((int)(num.m_x + 1), (int)(num.m_y + 1), (int)(num.m_z + 1));
	}

	float rand1dTo1d(float value, float mutator = 0.546)
	{

		float random = frac(sin(value + mutator) * m_seed);
		return random;
	}

	Vector3D rand1dTo3d(float value)
	{
		return Vector3D
		(
			rand1dTo1d(value, 3.9812),
			rand1dTo1d(value, 7.1536),
			rand1dTo1d(value, 5.7241)
		);
	}

	float rand2dTo1d(Vector2D value, Vector2D dotDir = Vector2D(12.9898, 78.233))
	{
		Vector2D smallValue(sin(value.m_x), sin(value.m_y));
		float random = Vector2D::dot(smallValue, dotDir);
		random = frac(sin(random) * m_seed);
		return random;
	}

	Vector2D rand2dTo2d(Vector2D value) {
		return Vector2D(
			rand2dTo1d(value, Vector2D(12.989, 78.233)),
			rand2dTo1d(value, Vector2D(39.346, 11.135))
		);
	}

	float rand3dTo1d(Vector3D value, Vector3D dotDir = Vector3D(12.9898, 78.233, 37.719))
	{
		//make value smaller to avoid artifacts
		Vector3D smallValue(sin(value.m_x), sin(value.m_y), sin(value.m_z));
		//get scalar value from 3d vector
		float random = Vector3D::dot(smallValue, dotDir);
		//make value more random by making it bigger and then taking the factional part
		random = frac(sin(random) * m_seed);
		return random;
	}

	Vector3D rand3dTo3d(Vector3D value)
	{
		return Vector3D(
			rand3dTo1d(value, Vector3D(12.989, 78.233, 37.719)),
			rand3dTo1d(value, Vector3D(39.346, 11.135, 83.155)),
			rand3dTo1d(value, Vector3D(73.156, 52.235, 09.151))
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
