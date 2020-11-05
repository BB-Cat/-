#include "Sprite.h"
#include "VertexMesh.h"
#include "TextureManager.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include "ConstantBufferSystem.h"
#include "Texture.h"
#include "Blend.h"
#include "../include/DirectXTK-master/Inc/WICTextureLoader.h"


Sprite::Sprite(const wchar_t* full_path):Resource(full_path)
{
	m_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(full_path);

	VertexMesh vertices[4] =
	{
		VertexMesh(Vector3D(0.0f, 1.0f, 0), Vector2D(0, 1), Vector3D(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vector3D(1.0f, 1.0f, 0), Vector2D(1, 1), Vector3D(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vector3D(0.0f, 0.0f, 0), Vector2D(0, 0), Vector3D(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vector3D(1.0f, 0.0f, 0), Vector2D(1, 0), Vector3D(0.0f, 0.0f, -1.0f))
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexMesh) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = vertices;

	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateBuffer(&bd, &init_data, &m_buffer);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
		{"POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA ,0},
		{ "TEXCOORD", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"NORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,  D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	UINT size_layout = ARRAYSIZE(layout);

	void* shader_byte_code;
	size_t size_byte_shader;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_byte_shader);

	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->
		CreateInputLayout(layout, size_layout, shader_byte_code, size_byte_shader, &m_layout);

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateDepthStencilState(&dsDesc, &m_dss);

	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;//
	rsDesc.CullMode = D3D11_CULL_NONE;//	ƒJƒŠƒ“ƒO
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0;
	rsDesc.SlopeScaledDepthBias = 0;
	rsDesc.DepthClipEnable = false;
	rsDesc.ScissorEnable = false;
	rsDesc.MultisampleEnable = false;
	rsDesc.AntialiasedLineEnable = false;
	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rsDesc, &m_rs);


}

Sprite::~Sprite()
{
	if(m_buffer) m_buffer->Release();
	if(m_dss) m_dss->Release();
	if(m_layout) m_layout->Release();
	if(m_rs) m_rs->Release();
}

void Sprite::renderSprite(Vector3D scale, Vector3D position, Vector3D rotation,
	Vector2D texture_pos, Vector2D texture_size, Vector2D origin)
{
	if (scale.m_x == 0.0f || scale.m_y == 0.0f) return;

	VertexMesh vertices[] =
	{
		VertexMesh(Vector3D(0.0f, 1.0f, 0), Vector2D(0, 1), Vector3D(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vector3D(1.0f, 1.0f, 0), Vector2D(1, 1), Vector3D(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vector3D(0.0f, 0.0f, 0), Vector2D(0, 0), Vector3D(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vector3D(1.0f, 0.0f, 0), Vector2D(1, 0), Vector3D(0.0f, 0.0f, -1.0f))
	};

	Vector2D image_size = m_tex->getSize();
	
	float left = texture_pos.m_x / image_size.m_x;
	float right = left + texture_size.m_x / image_size.m_x;
	float top = texture_pos.m_y / image_size.m_y;
	float bottom = top + texture_size.m_y / image_size.m_y;

	vertices[0].m_texcoord = Vector2D(left, top);
	vertices[1].m_texcoord = Vector2D(right, top);
	vertices[2].m_texcoord = Vector2D(left, bottom);
	vertices[3].m_texcoord = Vector2D(right, bottom);

	float modX = origin.m_x / texture_size.m_x;
	float modY = origin.m_y / texture_size.m_y;

	vertices[0].m_position = vertices[0].m_position - Vector3D(modX, modY, 0);
	vertices[1].m_position = vertices[1].m_position - Vector3D(modX, modY, 0);
	vertices[2].m_position = vertices[2].m_position - Vector3D(modX, modY, 0);
	vertices[3].m_position = vertices[3].m_position - Vector3D(modX, modY, 0);


	D3D11_MAPPED_SUBRESOURCE msr;
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertices, sizeof(vertices));
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context->Unmap(m_buffer, 0);

	//GraphicsEngine::get()->getShaderManager()->setPixelShader(PS::SPRITE);
	//GraphicsEngine::get()->getShaderManager()->setVertexShader(VS::MODEL);

	GraphicsEngine::get()->getShaderManager()->setPipeline(FLAT_TEX);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex);


	Matrix4x4 temp;
	temp.setIdentity();
	temp = applyTransformations(temp, scale, rotation, position);

	Matrix4x4 temp_placeholder[MAXBONES];
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTransformationBuffer(temp, temp_placeholder);

	BlendMode::get()->SetBlend(BlendType::ALPHA);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDepthStencilState(m_dss);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_rs);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(sizeof(VertexMesh), m_buffer, m_layout);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawTriangleStrip(4, 0);
}

Matrix4x4 Sprite::applyTransformations(const Matrix4x4& global, Vector3D scale, Vector3D rot, Vector3D translate)
{
	Matrix4x4 out = global;
	Matrix4x4 temp;
	temp.setIdentity();

	//Scale
	temp.setScale(scale);
	out *= temp;

	//Rotation
	temp.setIdentity();
	temp.setRotationZ(rot.m_z);
	out *= temp;
	temp.setIdentity();
	temp.setRotationX(rot.m_x);
	out *= temp;
	temp.setIdentity();
	temp.setRotationY(rot.m_y);
	out *= temp;

	//Translation
	out.setTranslation(translate);

	return out;

}
