#pragma once
#include "Vector3D.h"

class Instance
{
public:
	Instance() :m_position()
	{
	}

	Instance(Vector3D v) :m_position(v)
	{
	}


	~Instance()
	{
	}

public:
	Vector3D m_position;
};