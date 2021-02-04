#include "Mesh.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"

Mesh::Mesh(const wchar_t* full_path, D3D11_CULL_MODE culltype) : Resource(full_path)
{
	ID3D11Device* device = GraphicsEngine::get()->getRenderSystem()->m_d3d_device;
	HRESULT hr;

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = culltype;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rsDesc, &m_wire_rast);
	if (FAILED(hr))	assert(0 && "Error loading the wireframe rasterizer");

	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;//
	rsDesc.CullMode = culltype;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0;
	rsDesc.SlopeScaledDepthBias = 0;
	rsDesc.DepthClipEnable = false;
	rsDesc.ScissorEnable = false;
	rsDesc.MultisampleEnable = false;
	rsDesc.AntialiasedLineEnable = false;
	hr = device->CreateRasterizerState(&rsDesc, &m_solid_rast);
	if (FAILED(hr))	assert(0 && "Error loading the solid fill rasterizer");

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	hr = device->CreateDepthStencilState(&dsDesc, &m_depth_stencil);

}

Mesh::~Mesh()
{
	if(m_wire_rast) m_wire_rast->Release();
	if (m_solid_rast) m_solid_rast->Release();
	if(m_depth_stencil) m_depth_stencil->Release();
}

Matrix4x4 Mesh::applyTransformations(const Matrix4x4& global, Vec3 scale, Vec3 rot, Vec3 translate)
{
	Matrix4x4 out = global;
	Matrix4x4 temp;
	temp.setIdentity();

	//Scale
	temp.setScale(scale);
	out *= temp;

	//Rotation
	temp.setIdentity();
	temp.setRotationZ(rot.z);
	out *= temp;
	temp.setIdentity();
	temp.setRotationX(rot.x);
	out *= temp;
	temp.setIdentity();
	temp.setRotationY(rot.y);
	out *= temp;

	//Translation
	out.setTranslation(translate);

	return out;

}



