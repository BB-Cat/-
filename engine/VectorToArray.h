#pragma once
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include <vector>

class VectorToArray
{
public:
	VectorToArray(Vec2* v)
	{
		float* f = &v->x;
		vals.push_back(f);
		f = &v->y;
		vals.push_back(f);
	}
	VectorToArray(Vec3* v)
	{
		float* f = &v->x;
		vals.push_back(f);
		f = &v->y;
		vals.push_back(f);
		f = &v->z;
		vals.push_back(f);
	}
	VectorToArray(Vector4D* v)
	{
		float* f = &v->m_x;
		vals.push_back(f);
		f = &v->m_y;
		vals.push_back(f);
		f = &v->m_z;
		vals.push_back(f);
		f = &v->m_w;
		vals.push_back(f);
	}

	float* setArray()
	{
		return vals[0];
	}

public:
	std::vector<float*> vals;
};