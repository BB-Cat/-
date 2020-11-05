#include "PrimitiveCapsule.h"
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




PrimitiveCapsule::PrimitiveCapsule(float x_radius, float y_radius, int slice_count, int stack_count)
{
	std::vector<vertex> v_list;

	for (int i = 0; i <= stack_count; i++)
	{
		float theta = i * M_PI / stack_count;
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (int j = 0; j <= slice_count; j++)
		{
			float phi = j * 2 * M_PI / slice_count;
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			Vector3D v;
			float normX, normY, normZ;
			normX = cosPhi * sinTheta;
			normY = cosTheta;
			normZ = sinPhi * sinTheta;

			v.m_x = x_radius * normX;
			v.m_y = y_radius * normY;
			v.m_z = (x_radius + y_radius) / 2 * normZ;


			v_list.push_back(vertex(v, Vector3D(0.2f, 0.05f * j, 0.05f * i), Vector3D((float)i / stack_count, (float)j / slice_count, 0.2f)));
		}
	}

	UINT size_list = sizeof(vertex) * v_list.size();

	void* vertex_list = malloc(sizeof(vertex) * v_list.size());
	memcpy(vertex_list, &v_list[0], sizeof(vertex) * v_list.size());

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vertex_shader = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(vertex_list, sizeof(vertex), v_list.size(), shader_byte_code, size_shader);

	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	std::vector<unsigned int> i_list;

	for (int i = 0; i < stack_count; i++)
	{
		for (int j = 0; j < slice_count; j++)
		{
			int first = (i * (slice_count + 1)) + j;
			int second = first + slice_count + 1;

			i_list.push_back(first);
			i_list.push_back(second);
			i_list.push_back(first + 1);

			i_list.push_back(second);
			i_list.push_back(second + 1);
			i_list.push_back(first + 1);
		}
	}

	//UINT size_i_list = sizeof(unsigned int) * i_list.size();

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

PrimitiveCapsule::~PrimitiveCapsule()
{
	MyGeometricPrimitive::~MyGeometricPrimitive();
}