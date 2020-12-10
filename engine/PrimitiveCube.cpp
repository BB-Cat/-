#include "PrimitiveCube.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "MyConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

#include "Vector3D.h"
#include "Matrix4X4.h"

#include <vector>


PrimitiveCube::PrimitiveCube(float width, float height, float depth)
{
	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	std::vector<vertex> v_list;

	//POS1					 //COLOR1			//COLOR2	
	//FRONT FACE
	v_list.push_back(vertex(Vec3(-w2, -h2, -d2), Vec3(1, 0, 0), Vec3(0.2f, 0, 0)));
	v_list.push_back(vertex(Vec3(-w2, +h2, -d2), Vec3(1, 1, 0), Vec3(0.2f, 0.2f, 0)));
	v_list.push_back(vertex(Vec3(+w2, +h2, -d2), Vec3(1, 1, 0), Vec3(0.2f, 0.2f, 0)));
	v_list.push_back(vertex(Vec3(+w2, -h2, -d2), Vec3(1, 0, 0), Vec3(0.2f, 0, 0)));
	//BACK FACE
	v_list.push_back(vertex(Vec3(+w2, -h2, +d2), Vec3(0, 1, 0), Vec3(0, 0.2f, 0)));
	v_list.push_back(vertex(Vec3(+w2, +h2, +d2), Vec3(0, 1, 1), Vec3(0, 0.2f, 0.2f)));
	v_list.push_back(vertex(Vec3(-w2, +h2, +d2), Vec3(0, 1, 1), Vec3(0, 0.2f, 0.2f)));
	v_list.push_back(vertex(Vec3(-w2, -h2, +d2), Vec3(0, 1, 0), Vec3(0, 0.2f, 0)));


	UINT size_list = sizeof(vertex) * v_list.size();

	void* vertex_list = malloc(sizeof(vertex) * v_list.size());
	memcpy(vertex_list, &v_list[0], sizeof(vertex) * v_list.size());

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vertex_shader = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(vertex_list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	unsigned int index_list[] =
	{
		//FRONT SIDE
		0,1,2, //triangle1
		2,3,0, //triangle2
		//BACK SIDE
		4,5,6, //triangle3
		6,7,4, //triangle4
		//TOP SIDE
		1,6,5, //triangle5
		5,2,1, //triangle6
		//BOTTOM SIDE
		7,0,3, //triangle7
		3,4,7, //triangle8
		//RIGHT SIDE
		3,2,5, //triangle9
		5,4,3, //triangle10
		//LEFT SIDE
		7,6,1, //triangle11
		1,0,7, //triangle12

	};


	UINT size_index_list = ARRAYSIZE(index_list);
	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(index_list, size_index_list);



	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_pixel_shader = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;
	m_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));


	MyGeometricPrimitive::createRasterizerStates();
}

PrimitiveCube::~PrimitiveCube()
{
	MyGeometricPrimitive::~MyGeometricPrimitive();
}

