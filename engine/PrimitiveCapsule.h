#pragma once
#include "MyGeometricPrimitive.h"


class PrimitiveCapsule :
	public MyGeometricPrimitive
{
public:
	PrimitiveCapsule(float x_radius, float y_radius, int slice_count, int stack_count);
	~PrimitiveCapsule();

private:
};

