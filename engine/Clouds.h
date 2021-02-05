#pragma once
#include "Vector4D.h"
#include "ConstantBufferFormats.h"

struct cloud_struct
{
	int num_spheres;
	Vector4D sphere_data[MAX_SPHERES];
};