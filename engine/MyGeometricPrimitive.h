#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

#include "Prerequisites.h"
#include "Vector3D.h"

struct vertex;

class MyGeometricPrimitive
{
public:
	MyGeometricPrimitive();
	~MyGeometricPrimitive();

	void createRasterizerStates();

	void updateTransformation(RECT window_rect, Vector3D scale, Vector3D translate, Vector3D rotate);
	void render(RECT window_rect, Vector3D scale, Vector3D translate, Vector3D rotate, bool isWireframe);

protected:
	VertexBuffer* m_vertex_buffer;
	VertexShader* m_vertex_shader;
	PixelShader* m_pixel_shader;
	MyConstantBuffer* m_constant_buffer;
	IndexBuffer* m_index_buffer;

	ID3D11RasterizerState*		m_line_raster_state;
	ID3D11RasterizerState*		m_fill_raster_state;
	ID3D11DepthStencilState*	m_depth_stencil_state;

};