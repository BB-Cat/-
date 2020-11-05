#include "PrimitiveCylinder.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "MyConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

#include "Vector3D.h"
#include "Matrix4X4.h"
#include "MathConstants.h"

#include <vector>





PrimitiveCylinder::PrimitiveCylinder(float bottom_radius, float top_radius, float height, int slice_count, int stack_count)
{
	std::vector<vertex> v_list;

	float stack_height = height / stack_count;
	float radius_step = (top_radius - bottom_radius) / stack_count;
	int ring_count = stack_count + 1;

	for (int i = 0; i < ring_count; i++)
	{
		float y = -0.5f * height + i * stack_height;
		float r = bottom_radius + i * radius_step;
		float theta = 2.0f * M_PI / slice_count;

		for (int j = 0; j <= slice_count; j++)
		{
			float cosTheta = cosf(j * theta);
			float sinTheta = sinf(j * theta);

			Vector3D v;
			v.m_x = r * cosTheta;
			v.m_y = y;
			v.m_z = r * sinTheta;

			v_list.push_back(vertex(v, Vector3D(i * 0.05f, 0.2f, 0.2f), Vector3D(0, j * 0.05f, 0.2f)));
		}
	}

	std::vector<unsigned int> i_list;

	int ring_vertex_count = slice_count;
	for (int i = 0; i < stack_count; i++)
	{
		for (int j = 0; j < slice_count; j++)
		{
			i_list.push_back(i * ring_vertex_count + j);
			i_list.push_back((i + 1) * ring_vertex_count + j);
			i_list.push_back((i + 1) * ring_vertex_count + j + 1);

			i_list.push_back(i * ring_vertex_count + j);
			i_list.push_back((i + 1) * ring_vertex_count + j + 1);
			i_list.push_back(i * ring_vertex_count + j + 1);
		}
	}

	//buildTopCap(top_radius, height, slice_count, &v_list, &i_list);
	//buildBottomCap(bottom_radius, height, slice_count, v_list, i_list);

	////////////////////////////////////////////////////////

	//build the top cap of the cylinder

	int base_index = v_list.size();

	float y = 0.5f * height;
	float theta = 2.0f * M_PI / slice_count;

	for (int i = 0; i <= slice_count; i++)
	{
		float x = top_radius * cosf(i * theta);
		float z = top_radius * sinf(i * theta);

		v_list.push_back(vertex(Vector3D(x, y, z), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.4f, 0.4f, 0.4f)));
	}

	v_list.push_back(vertex(Vector3D(0, y, 0), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.4f, 0.4f, 0.4f)));
	int center_index = v_list.size() - 1;

	for (int i = 0; i < slice_count; i++)
	{
		i_list.push_back(center_index);
		i_list.push_back(base_index + i + 1);
		i_list.push_back(base_index + i);
	}

	//build the bottom cap of the cylinder

	base_index = v_list.size();

	y = -0.5f * height;
	theta = 2.0f * M_PI / slice_count;

	for (int i = 0; i <= slice_count; i++)
	{
		float x = bottom_radius * cosf(i * theta);
		float z = bottom_radius * sinf(i * theta);

		v_list.push_back(vertex(Vector3D(x, y, z), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.4f, 0.4f, 0.4f)));
	}

	v_list.push_back(vertex(Vector3D(0, y, 0), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.4f, 0.4f, 0.4f)));
	center_index = v_list.size() - 1;

	for (int i = 0; i < slice_count; i++)
	{
		i_list.push_back(center_index);
		i_list.push_back(base_index + i);
		i_list.push_back(base_index + i + 1);
	}

	////////////////////////////////////////////////////////

	UINT size_list = sizeof(vertex) * v_list.size();

	void* vertex_list = malloc(sizeof(vertex) * v_list.size());
	memcpy(vertex_list, &v_list[0], sizeof(vertex) * v_list.size());

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vertex_shader = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(vertex_list, sizeof(vertex), v_list.size(), shader_byte_code, size_shader);

	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	/*UINT size_i_list = sizeof(unsigned int) * i_list.size();*/

	void* index_list = malloc(sizeof(unsigned int) * i_list.size());
	memcpy(index_list, &i_list[0], sizeof(unsigned int) * i_list.size());
	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(index_list, i_list.size());

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_pixel_shader = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;
	m_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));

	MyGeometricPrimitive::createRasterizerStates();
}

PrimitiveCylinder::~PrimitiveCylinder()
{
	MyGeometricPrimitive::~MyGeometricPrimitive();
}

void PrimitiveCylinder::buildTopCap(float top_radius, float height, int slice_count, std::vector<vertex> v_list, std::vector<unsigned int> i_list)
{
	int base_index = v_list.size();

	float y = 0.5f * height;
	float theta = 2.0f * M_PI / slice_count;

	for (int i = 0; i <= slice_count; i++)
	{
		float x = top_radius * cosf(i * theta);
		float z = top_radius * sinf(i * theta);

		v_list.push_back(vertex(Vector3D(x, y, z), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.4f, 0.4f, 0.4f)));
	}

	v_list.push_back(vertex(Vector3D(0, y, 0), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.4f, 0.4f, 0.4f)));
	int center_index = v_list.size() - 1;

	for (int i = 0; i < slice_count; i++)
	{
		i_list.push_back(center_index);
		i_list.push_back(base_index + i + 1);
		i_list.push_back(base_index + i);
	}
}

void PrimitiveCylinder::buildBottomCap(float bottom_radius, float height, int slice_count, std::vector<vertex> v_list, std::vector<unsigned int> i_list)
{
	int base_index = v_list.size();

	float y = -0.5f * height;
	float theta = 2.0f * M_PI / slice_count;

	for (int i = 0; i <= slice_count; i++)
	{
		float x = bottom_radius * cosf(i * theta);
		float z = bottom_radius * sinf(i * theta);

		v_list.push_back(vertex(Vector3D(x, y, z), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.4f, 0.4f, 0.4f)));
	}

	v_list.push_back(vertex(Vector3D(0, y, 0), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.4f, 0.4f, 0.4f)));
	int center_index = v_list.size() - 1;

	for (int i = 0; i < slice_count; i++)
	{
		i_list.push_back(center_index);
		i_list.push_back(base_index + i);
		i_list.push_back(base_index + i + 1);
	}
}
