#pragma once
#include "Vector3D.h"

class Instance
{
public:
	Instance() :m_position()
	{
	}

	Instance(Vec3 v) :m_position(v)
	{
	}


	~Instance()
	{
	}

public:
	Vec3 m_position;
};