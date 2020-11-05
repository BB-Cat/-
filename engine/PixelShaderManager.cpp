#include "PixelShaderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"

PixelShaderManager::PixelShaderManager()
{
}

PixelShaderManager::~PixelShaderManager()
{
}

void PixelShaderManager::compileShaders()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_pixel_shader_1tex = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShaderNoTexture.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_pixel_shader_notex = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
}

void PixelShaderManager::setPixelShader(int type)
{
	switch (type)
	{
	case NO_TEX:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_pixel_shader_notex);
		break;
	case ONE_TEX:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_pixel_shader_1tex);
		break;
	}
}
