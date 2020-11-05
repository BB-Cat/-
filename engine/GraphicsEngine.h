#pragma once
#include <d3d11.h>
//#include <d3dcompiler.h>
#include "Prerequisites.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "SpriteManager.h"
#include "StaticMeshManager.h"
#include "SkinnedMeshManager.h"
#include "ShaderManager.h"


//singleton class which handles all graphics resources
class GraphicsEngine
{
public:
	//get location of the render system
	RenderSystem* getRenderSystem();
	//get the location of the lighting system
	ConstantBufferSystem* getConstantBufferSystem();
	//get the location of the texture manager
	TextureManager* getTextureManager();
	//get the location of the sprite manager
	SpriteManager* getSpriteManager();
	//get the location of the static mesh manager
	StaticMeshManager* getStaticMeshManager();
	//get the location of the skinned mesh manager
	SkinnedMeshManager* getSkinnedMeshManager();
	//get the location of the skinned mesh manager
	ShaderManager* getShaderManager();
	//get the byte code and size of the shader for vertex meshes
	void getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size);

public:
	//create the text renderer
	void createTextRenderer(HWND* hwnd, SwapChainPtr swapchain);
	//render a string at the target location on screen
	void renderText(const wchar_t text[], Vector2D pos, Vector2D size, int color);
	//change the text renderer's font size
	void changeTextSize(float size);

public:
	//function to retrieve address to static graphics engine
	static GraphicsEngine* get();
	static void create();
	static void release();

private:
	//initialize the graphics engine and DirectX11 Device
	GraphicsEngine();
	//release the graphics engine
	~GraphicsEngine();

private:
	RenderSystem* m_render_system = nullptr;
	ConstantBufferSystem* m_constant_buffer_system = nullptr;
	TextureManager* m_texture_manager = nullptr;
	SpriteManager* m_sprite_manager = nullptr;
	StaticMeshManager* m_static_mesh_manager = nullptr;
	SkinnedMeshManager* m_skinned_mesh_manager = nullptr;
	ShaderManager* m_shader_manager = nullptr;
	TextRenderer* m_text = nullptr;

	static GraphicsEngine* m_engine;

	unsigned char m_mesh_layout_byte_code[1024];
	size_t m_mesh_layout_size = 0;
};

