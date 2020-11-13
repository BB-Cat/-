#pragma once
#include "Resource.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Matrix4X4.h"

//base mesh class which contains data that all inherited mesh classes will use
class Mesh :public Resource
{
public:
	Mesh(const wchar_t* full_path, D3D11_CULL_MODE culltype = D3D11_CULL_BACK);
	~Mesh();



public:
	virtual void renderMesh(float elapsed_time, Vector3D scale, Vector3D position, Vector3D rotation, 
		int shader, bool is_textured = true, float animation_speed = 1.0f) = 0;


protected:
	VertexBufferPtr m_vertex_buffer;
	IndexBufferPtr m_index_buffer;
	ID3D11DepthStencilState* m_depth_stencil;
	ID3D11RasterizerState* m_solid_rast;
	ID3D11RasterizerState* m_wire_rast;

	//ID3D11RasterizerState* m_wire;



	Matrix4x4 applyTransformations(const Matrix4x4& global, Vector3D scale, Vector3D rot, Vector3D translate);
protected:
	friend class DeviceContext;

};
