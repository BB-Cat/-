#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Prerequisites.h"


//singleton class which handles all graphics resources
class RenderSystem
{
public:
	//initialize the graphics engine and DirectX11 Device
	RenderSystem();
	//release the graphics engine
	~RenderSystem();
	//initialize ImGui Assets (this needs to be moved somewhere better but for now its here)
	void initializeImGui(HWND hwnd);

public:
	DeviceContextPtr getImmediateDeviceContext();
	GBufferPtr getGBuffer();
public:

	SwapChainPtr createSwapChain(HWND hwnd, UINT width, UINT height);
	VertexBufferPtr createVertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader);
	IndexBufferPtr createIndexBuffer(void* list_indices, UINT size_list);
	MyConstantBufferPtr createConstantBuffer(void* buffer, UINT size_buffer);
	VertexShaderPtr createVertexShader(const void* shader_byte_code, size_t byte_code_size);
	GeometryShaderPtr createGeometryShader(const void* shader_byte_code, size_t byte_code_size);
	HullShaderPtr createHullShader(const void* shader_byte_code, size_t byte_code_size);
	DomainShaderPtr createDomainShader(const void* shader_byte_code, size_t byte_code_size);
	PixelShaderPtr createPixelShader(const void* shader_byte_code, size_t byte_code_size);
	ComputeShaderPtr createComputeShader(const void* shader_byte_code, size_t byte_code_size,
		size_t input_structure_size, size_t output_structure_size, void* data, UINT input_count);
	SamplerPtr createSamplerState();
	void createGBuffer(UINT width, UINT height);

public:
	bool compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	bool compileGeometryShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	bool compileHullShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	bool compileDomainShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	bool compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	bool compileComputeShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	void releaseCompiledShader();

	bool compileVSIncludeFile(const wchar_t* filename, void** include_byte_code, size_t* include_byte_code_size);

private:
	DeviceContextPtr m_imm_device_context; //immediate device context
	GBufferPtr m_gbuffer; //deferred rendering class for shadow mapping and other deferred rendering features

private:
	ID3D11Device* m_d3d_device;
	D3D_FEATURE_LEVEL m_feature_level;

private:
	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;
	ID3D11DeviceContext* m_imm_context;

private:
private:
	ID3DBlob* m_blob = nullptr;
	ID3DBlob* m_vsblob = nullptr;
	ID3DBlob* m_psblob = nullptr;
	ID3D11VertexShader* m_vs = nullptr;
	ID3D11PixelShader* m_ps = nullptr;


private:
	friend class SwapChain;
	friend class DeviceContext;
	friend class GBuffer;
	friend class VertexBuffer;
	friend class IndexBuffer;
	friend class VertexShader;
	friend class PixelShader;
	friend class GeometryShader;
	friend class HullShader;
	friend class DomainShader;
	friend class ComputeShader;
	friend class Sampler;
	friend class MyConstantBuffer;
	friend class Texture;
	friend class Texture3D;
	friend class StaticMesh;
	friend class ConstantBufferSystem;
	friend class BlendMode;
	friend class TextRenderer;
	friend class Sprite;
	friend class Mesh;
	friend class TerrainManager;
	friend class WorldObject;

	//temp - for testing scene 13 
	friend class Scene13;
};

