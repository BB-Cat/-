#pragma once
#include "MyGeometricPrimitive.h"


class PrimitiveSphere :
	public MyGeometricPrimitive
{
public:
	PrimitiveSphere(float radius, int slice_count, int stack_count);
	~PrimitiveSphere();

private:
};

