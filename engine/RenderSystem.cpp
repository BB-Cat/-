#include "RenderSystem.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "MyConstantBuffer.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "PixelShader.h"
#include "ComputeShader.h"
#include "TextureComputeShader.h"
#include "Sampler.h"
#include "GBuffer.h"


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include <d3dcompiler.h>


RenderSystem::RenderSystem()
{
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT num_driver_types = ARRAYSIZE(driver_types);

	HRESULT res = 0;


	for (UINT driver_type_index = 0; driver_type_index < num_driver_types;)
	{
		D3D_FEATURE_LEVEL feature_levels[] =
		{
		D3D_FEATURE_LEVEL_11_0,
		};
		UINT num_feature_levels = ARRAYSIZE(feature_levels);

		//this is necessary for direct2D to work
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		res = D3D11CreateDevice(NULL, driver_types[driver_type_index], NULL, creationFlags, feature_levels, num_feature_levels, D3D11_SDK_VERSION,
			&m_d3d_device, &m_feature_level, &m_imm_context);

		if (SUCCEEDED(res))
		{
			break;
		}

		++driver_type_index;
	}

	if (FAILED(res))
	{
		throw std::exception("RenderSystem not successfully created");
	}

	m_imm_device_context = std::make_shared <DeviceContext>(m_imm_context, this);

	m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);

	createSamplerState();


}

RenderSystem::~RenderSystem()
{
	m_d3d_device->Release();
	m_dxgi_device->Release();
	m_dxgi_adapter->Release();
	m_dxgi_factory->Release();
}

void RenderSystem::initializeImGui(HWND hwnd)
{
	//ImGUI setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(m_d3d_device, m_imm_device_context->m_device_context);
	ImGui::StyleColorsClassic();
}

void RenderSystem::releaseImGui()
{
	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


SwapChainPtr RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height)
{
	SwapChainPtr sc = nullptr;
	try
	{
		sc=std::make_shared<SwapChain>(hwnd, width, height, this);
	}
	catch (...) {}

	return sc;
}

DeviceContextPtr RenderSystem::getImmediateDeviceContext()
{
	return this->m_imm_device_context;
}

GBufferPtr RenderSystem::getGBuffer()
{
	return this->m_gbuffer;
}

VertexBufferPtr RenderSystem::createVertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader)
{
	VertexBufferPtr vb = nullptr;
	try
	{
		vb = std::make_shared <VertexBuffer>(list_vertices, size_vertex, size_list, shader_byte_code, size_byte_shader, this);
	}
	catch (...) {}

	return vb;
}

IndexBufferPtr RenderSystem::createIndexBuffer(void* list_indices, UINT size_list)
{
	IndexBufferPtr ib = nullptr;
	try
	{
		ib = std::make_shared <IndexBuffer>(list_indices, size_list, this);
	}
	catch (...) {}

	return ib;
}

MyConstantBufferPtr RenderSystem::createConstantBuffer(void* buffer, UINT size_buffer)
{
	MyConstantBufferPtr cb = nullptr;
	try
	{
		cb = std::make_shared <MyConstantBuffer>(buffer, size_buffer, this);
	}
	catch (...) {}

	return cb;
}

VertexShaderPtr RenderSystem::createVertexShader(const void* shader_byte_code, size_t byte_code_size)
{
	VertexShaderPtr vs = nullptr;
	try
	{
		vs = std::make_shared <VertexShader>(shader_byte_code, byte_code_size, this);
	}
	catch (...) {}

	return vs;
}

GeometryShaderPtr RenderSystem::createGeometryShader(const void* shader_byte_code, size_t byte_code_size)
{
	GeometryShaderPtr gs = nullptr;
	try
	{
		gs = std::make_shared <GeometryShader>(shader_byte_code, byte_code_size, this);
	}
	catch (...) {}

	return gs;
}

HullShaderPtr RenderSystem::createHullShader(const void* shader_byte_code, size_t byte_code_size)
{
	HullShaderPtr hs = nullptr;
	try
	{
		hs = std::make_shared <HullShader>(shader_byte_code, byte_code_size, this);
	}
	catch (...) {}

	return hs;
}

DomainShaderPtr RenderSystem::createDomainShader(const void* shader_byte_code, size_t byte_code_size)
{
	DomainShaderPtr ds = nullptr;
	try
	{
		ds = std::make_shared <DomainShader>(shader_byte_code, byte_code_size, this);
	}
	catch (...) {}

	return ds;
}

PixelShaderPtr RenderSystem::createPixelShader(const void* shader_byte_code, size_t byte_code_size)
{
	PixelShaderPtr ps = nullptr;
	try
	{
		ps = std::make_shared <PixelShader>(shader_byte_code, byte_code_size, this);
	}
	catch (...) {}

	return ps;
}

ComputeShaderPtr RenderSystem::createComputeShader(const void* shader_byte_code, size_t byte_code_size,
	size_t input_structure_size, size_t output_structure_size, void* data, UINT input_count)
{
	ComputeShaderPtr cs = nullptr;
	try
	{
		cs = std::make_shared <ComputeShader>(shader_byte_code, byte_code_size, this,
			input_structure_size, output_structure_size, data, input_count);
	}
	catch (...) {}

	return cs;
}

TextureComputeShaderPtr RenderSystem::createTextureComputeShader(const void* shader_byte_code, 
	size_t byte_code_size, Vec2 dimensions)
{
	TextureComputeShaderPtr cs = nullptr;
	try
	{
		cs = std::make_shared <TextureComputeShader>(shader_byte_code, byte_code_size, this,
			dimensions);
	}
	catch (...) {}

	return cs;
}


SamplerPtr RenderSystem::createSamplerState()
{
	SamplerPtr s = nullptr;
	try
	{
		s = std::make_shared <Sampler>(this);
	}
	catch (...) {}

	return s;
}

void RenderSystem::createGBuffer(UINT width, UINT height)
{
	GBufferPtr g = nullptr;
	try
	{
		g = std::make_shared <GBuffer>(width, height);
	}
	catch (...) {}


	m_gbuffer = g;
}

bool RenderSystem::compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{

	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "vs_5_0", 0, 0, &m_blob, &error_blob)))
	{
		if (error_blob) error_blob->Release();
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

bool RenderSystem::compileGeometryShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "gs_5_0", 0, 0, &m_blob, &error_blob)))
	{
		if (error_blob) error_blob->Release();
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

bool RenderSystem::compileHullShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "hs_5_0", 0, 0, &m_blob, &error_blob)))
	{
		if (error_blob) error_blob->Release();
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

bool RenderSystem::compileDomainShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "ds_5_0", 0, 0, &m_blob, &error_blob)))
	{
		if (error_blob) error_blob->Release();
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

bool RenderSystem::compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "ps_5_0", 0, 0, &m_blob, &error_blob)))
	{
		//if (error_blob)
		//	MessageBoxA(0, (char*)error_blob->GetBufferPointer(), NULL, MB_OK);
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

bool RenderSystem::compileComputeShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "cs_5_0", 0, 0, &m_blob, &error_blob)))
	{
		if (error_blob) MessageBoxA(0, (char*)error_blob->GetBufferPointer(), NULL, MB_OK);
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

void RenderSystem::releaseCompiledShader()
{
	if (m_blob) m_blob->Release();
}

bool RenderSystem::compileVSIncludeFile(const wchar_t* filename, void** include_byte_code, size_t* include_byte_code_size)
{


	return false;
}


