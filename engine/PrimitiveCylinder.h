#pragma once
#include "MyGeometricPrimitive.h"
#include <vector>

struct vertex;

class PrimitiveCylinder :
	public MyGeometricPrimitive
{
public:
	PrimitiveCylinder(float bottom_radius, float top_radius, float height, int slice_count, int stack_count);
	~PrimitiveCylinder();

	void buildTopCap(float top_radius, float height, int slice_count, std::vector<vertex> v_list, std::vector<unsigned int> i_list);
	void buildBottomCap(float bottom_radius, float height, int slice_count, std::vector<vertex> v_list, std::vector<unsigned int> i_list);
};

