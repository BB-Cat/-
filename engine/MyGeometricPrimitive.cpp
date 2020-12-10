#include "MyGeometricPrimitive.h"

#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "MyConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

#include <sstream>
#include "Vector3D.h"
#include "Matrix4X4.h"



MyGeometricPrimitive::MyGeometricPrimitive()
{
}

MyGeometricPrimitive::~MyGeometricPrimitive()
{
	if (m_line_raster_state)	m_line_raster_state->Release();
	if (m_fill_raster_state)	m_fill_raster_state->Release();
	if (m_depth_stencil_state)	m_depth_stencil_state->Release();
}

void MyGeometricPrimitive::createRasterizerStates()
{
	//***  rasterizer state settings  ***//
	////////////////////////////////////////////////////////////
	//line rasterizer
	D3D11_RASTERIZER_DESC rast_desc;
	rast_desc.FillMode = D3D11_FILL_WIREFRAME;
	rast_desc.CullMode = D3D11_CULL_BACK;
	rast_desc.FrontCounterClockwise = false;
	rast_desc.DepthBias = 0;
	rast_desc.DepthBiasClamp = 0.0f;
	rast_desc.SlopeScaledDepthBias = 0.0f;
	rast_desc.DepthClipEnable = true;
	rast_desc.ScissorEnable = false;
	rast_desc.MultisampleEnable = false;
	rast_desc.AntialiasedLineEnable = false;

	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rast_desc, &m_line_raster_state);

	//fill rasterizer
	rast_desc.FillMode = D3D11_FILL_SOLID;
	rast_desc.CullMode = D3D11_CULL_BACK;
	rast_desc.FrontCounterClockwise = false;
	rast_desc.DepthBias = 0;
	rast_desc.DepthBiasClamp = 0.0f;
	rast_desc.SlopeScaledDepthBias = 0.0f;
	rast_desc.DepthClipEnable = true;
	rast_desc.ScissorEnable = false;
	rast_desc.MultisampleEnable = false;
	rast_desc.AntialiasedLineEnable = false;

	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rast_desc, &m_fill_raster_state);

	////////////////////////////////////////////////////////////

	//depth stencil settings --- *** very little reference to go on so theres a high chance this spot is bugging out ***
	D3D11_DEPTH_STENCIL_DESC ds_desc;

	// Depth test parameters
	ds_desc.DepthEnable = TRUE;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	ds_desc.StencilEnable = FALSE;
	ds_desc.StencilReadMask = 0xFF;
	ds_desc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	ds_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
	ds_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	ds_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_DECR;
	ds_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateDepthStencilState(&ds_desc, &m_depth_stencil_state);

}

void MyGeometricPrimitive::updateTransformation(RECT window_rect, Vec3 scale, Vec3 translate, Vec3 rotate)
{
	constant cc;
	cc.m_time = ::GetTickCount();

	Matrix4x4 temp;

	//!! the order of transformations is very important!
	//cc.m_world.setScale(Vector3D::lerp(Vector3D(0.5f, 0.5f, 0), Vector3D(1.0f, 1.0f, 0), (sin(m_delta_scale)+1.0f)/2.0f));

	//cc.m_world *= temp;
	temp.setTranslation(translate);
	cc.m_world.setScale(scale);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationZ(rotate.z);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(rotate.y);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationX(rotate.x);
	cc.m_world *= temp;


	cc.m_view.setIdentity();
	cc.m_projection.setOrthoLH
	(
		(window_rect.right - window_rect.left) / 300.0f,
		(window_rect.bottom - window_rect.top) / 300.0f,
		-4.0f,
		+4.0f
	);

	m_constant_buffer->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

void MyGeometricPrimitive::render(RECT window_rect, Vec3 scale, Vec3 translate, Vec3 rotate, bool isWireframe)
{
	updateTransformation(window_rect, scale, translate, rotate);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vertex_shader, m_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_pixel_shader, m_constant_buffer);

	//set the rasterizer state
	if(isWireframe)  GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_line_raster_state);
	else GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_fill_raster_state);

	//set default shader in the graphics pipeline
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vertex_shader);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_pixel_shader);

	//set the vertices which will be drawn
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);

	//set the index for the vertices which will be drawn
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_index_buffer);

	//draw the vertices
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(m_index_buffer->getSizeIndexList(), 0, 0);
}

