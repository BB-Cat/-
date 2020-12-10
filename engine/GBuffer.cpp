#include "GBuffer.h"
#include "AppWindow.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include "Blend.h"

#include "SwapChain.h"
#include "VertexMesh.h"


GBuffer::GBuffer(UINT width, UINT height)
{
	////////////////////////////////////////////////////
	//      Prepare the Resource Views 
	////////////////////////////////////////////////////

	//Generate the render target textures.
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (UINT i = 0; i < NUM_BUFFERS; i++)
		(GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&textureDesc, NULL, &m_render_targets[i].texture));

	//Generate the render target views.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	for (UINT i = 0; i < NUM_BUFFERS; i++)
		GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRenderTargetView(m_render_targets[i].texture, &renderTargetViewDesc, &m_render_targets[i].renderTargetView);

	//Generate the shader resource views.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (UINT i = 0; i < NUM_BUFFERS; i++)
		GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_render_targets[i].texture, &shaderResourceViewDesc, &m_render_targets[i].shaderResourceView);

	//Generate the depth stencil buffer texture.
	ID3D11Texture2D* depthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc{};
	depthStencilBufferDesc.Width = width;
	depthStencilBufferDesc.Height = height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&depthStencilBufferDesc, NULL, &depthStencilTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &m_depth_stencil_view);

	depthStencilTexture->Release();


	D3D11_RASTERIZER_DESC rast_desc;
	rast_desc.FillMode = D3D11_FILL_SOLID;
	rast_desc.CullMode = D3D11_CULL_BACK;
	rast_desc.FrontCounterClockwise = FALSE;
	rast_desc.DepthBias = 0;
	rast_desc.DepthBiasClamp = 0.0f;
	rast_desc.SlopeScaledDepthBias = 0.0f;
	rast_desc.DepthClipEnable = true;
	rast_desc.ScissorEnable = false;
	rast_desc.MultisampleEnable = false;
	rast_desc.AntialiasedLineEnable = false;

	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rast_desc, &m_rasterizer_state);

	//////////////////////////////////////////////
	//		Rendering member initialization
	//////////////////////////////////////////////
	VertexMesh vertices[4] =
	{
		VertexMesh(Vec3(0.0f, 1.0f, 0), Vec2(0, 1), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(1.0f, 1.0f, 0), Vec2(1, 1), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(0.0f, 0.0f, 0), Vec2(0, 0), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(1.0f, 0.0f, 0), Vec2(1, 0), Vec3(0.0f, 0.0f, -1.0f))
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

GBuffer::~GBuffer()
{
	if (m_depth_stencil_view) m_depth_stencil_view->Release();
	for (int i = 0; i < NUM_BUFFERS; i++)
	{
		m_render_targets[i].renderTargetView->Release();
		m_render_targets[i].shaderResourceView->Release();
		m_render_targets[i].texture->Release();
	}

	if (m_buffer) m_buffer->Release();
	if (m_dss) m_dss->Release();
	if (m_layout) m_layout->Release();
	if (m_rs) m_rs->Release();

}

void GBuffer::renderToSwapChain(int final_pass_shader_type,Vec2 pos, Vec2 scale, Vec2 rot)
{
	VertexMesh vertices[] =
	{
		VertexMesh(Vec3(0.0f, 0.0f, 0), Vec2(0, 1), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(2.0f, 0.0f, 0), Vec2(1, 1), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(0.0f, 2.0f, 0), Vec2(0, 0), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(2.0f, 2.0f, 0), Vec2(1, 0), Vec3(0.0f, 0.0f, -1.0f))
	};

	for (int i = 0; i < 4; i++)
	{
		vertices[i].m_position = vertices[i].m_position * Vec3(scale.x, scale.y, 1);
		vertices[i].m_position += pos;
		vertices[i].m_position -= Vec3(1, 1, 0);
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertices, sizeof(vertices));
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context->Unmap(m_buffer, 0);

	GraphicsEngine::get()->getShaderManager()->setFinalPassShader(final_pass_shader_type);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setGBufferSRVs();
	//GraphicsEngine::get()->getShaderManager()->setVertexShader(VS::MODEL);
	//GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex);
	//GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_render_targets[0].shaderResourceView);



	BlendMode::get()->SetBlend(BlendType::ALPHA);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDepthStencilState(m_dss);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_rs);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(sizeof(VertexMesh), m_buffer, m_layout);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawTriangleStrip(4, 0);
}

void GBuffer::renderToSwapChain(int buffer, int final_pass_shader_type, Vec2 pos, Vec2 scale, Vec2 rot)
{
	VertexMesh vertices[] =
	{
		VertexMesh(Vec3(0.0f, 0.0f, 0), Vec2(0, 1), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(2.0f, 0.0f, 0), Vec2(1, 1), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(0.0f, 2.0f, 0), Vec2(0, 0), Vec3(0.0f, 0.0f, -1.0f)),
		VertexMesh(Vec3(2.0f, 2.0f, 0), Vec2(1, 0), Vec3(0.0f, 0.0f, -1.0f))
	};

	for (int i = 0; i < 4; i++)
	{
		vertices[i].m_position = vertices[i].m_position * Vec3(scale.x, scale.y, 1);
		vertices[i].m_position += pos;
		vertices[i].m_position -= Vec3(1, 1, 0);
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertices, sizeof(vertices));
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context->Unmap(m_buffer, 0);

	GraphicsEngine::get()->getShaderManager()->setFinalPassShader(final_pass_shader_type);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_render_targets[buffer].shaderResourceView);



	BlendMode::get()->SetBlend(BlendType::ALPHA);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDepthStencilState(m_dss);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_rs);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(sizeof(VertexMesh), m_buffer, m_layout);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawTriangleStrip(4, 0);
}

void GBuffer::setShadowMapTex()
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setShadowMapPS(
		m_render_targets[3].shaderResourceView,
		m_render_targets[4].shaderResourceView,
		m_render_targets[5].shaderResourceView
	);

}
