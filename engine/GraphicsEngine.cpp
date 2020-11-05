#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "ConstantBufferSystem.h"
#include "TextRenderer.h"






#include <exception>

GraphicsEngine* GraphicsEngine::m_engine = nullptr;

GraphicsEngine::GraphicsEngine()
{
	try
	{
		m_render_system = new RenderSystem();
	}
	catch (...) { throw std::exception("GraphicsEngine not successfully created"); }

	try
	{
		/* The texture manager requires information from the render system, 
		so it must be allocated after the render system pointer*/
		m_texture_manager = new TextureManager();
	}
	catch (...) { throw std::exception("TextureManager not successfully created"); }

	try
	{
		m_sprite_manager = new SpriteManager();
	}
	catch (...) { throw std::exception("TextureManager not successfully created"); }

	try
	{
		m_static_mesh_manager = new StaticMeshManager();
	}
	catch (...) { throw std::exception("StaticMeshManager not successfully created"); }

	try
	{
		m_skinned_mesh_manager = new SkinnedMeshManager();
	}
	catch (...) { throw std::exception("SkinnedMeshManager not successfully created"); }

	try
	{
		m_constant_buffer_system = new ConstantBufferSystem();
	}
	catch (...) { throw std::exception("GraphicsEngine not successfully created"); }

	try
	{
		m_shader_manager = new ShaderManager();
	}
	catch (...) { throw std::exception("PixelShaderManager not successfully created"); }


	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	m_render_system->compileVertexShader(L"VertexMeshLayoutShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	::memcpy(m_mesh_layout_byte_code, shader_byte_code, size_shader);
	m_mesh_layout_size = size_shader;
	m_render_system->releaseCompiledShader();



}

GraphicsEngine::~GraphicsEngine()
{
	m_engine = nullptr;
	delete m_static_mesh_manager;
	delete m_texture_manager;
	delete m_render_system;
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return m_render_system;
}

ConstantBufferSystem* GraphicsEngine::getConstantBufferSystem()
{
	return m_constant_buffer_system;
}

TextureManager* GraphicsEngine::getTextureManager()
{
	return m_texture_manager;
}

SpriteManager* GraphicsEngine::getSpriteManager()
{
	return m_sprite_manager;
}

StaticMeshManager* GraphicsEngine::getStaticMeshManager()
{
	return m_static_mesh_manager;
}

SkinnedMeshManager* GraphicsEngine::getSkinnedMeshManager()
{
	return m_skinned_mesh_manager;
}

ShaderManager* GraphicsEngine::getShaderManager()
{
	return m_shader_manager;
}

void GraphicsEngine::getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size)
{
	*byte_code = m_mesh_layout_byte_code;
	*size = m_mesh_layout_size;
}

void GraphicsEngine::createTextRenderer(HWND* hwnd, SwapChainPtr swapchain)
{
	m_text = new TextRenderer(hwnd, swapchain);
}

void GraphicsEngine::renderText(const wchar_t text[], Vector2D pos, Vector2D size, int color)
{
	m_text->render(text, pos, size, color);
}

void GraphicsEngine::changeTextSize(float size)
{
	m_text->changeFontSize(size);
}

GraphicsEngine* GraphicsEngine::get()
{
	return m_engine;
}

void GraphicsEngine::create()
{
	if (GraphicsEngine::m_engine) throw std::exception("GraphicsEngine was already created");
	GraphicsEngine::m_engine = new GraphicsEngine();
}

void GraphicsEngine::release()
{
	if (!GraphicsEngine::m_engine) return;
	delete GraphicsEngine::m_engine;
}

