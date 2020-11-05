#include "PixelShader.h"
#include "RenderSystem.h"
#include <exception>

PixelShader::PixelShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system) : m_system(system)
{
	if (!SUCCEEDED(m_system->m_d3d_device->CreatePixelShader(shader_byte_code, byte_code_size, nullptr, &m_pixel_shader)))
	{
		throw std::exception("PixelShader not created successfully");
	}
}

PixelShader::~PixelShader()
{
	m_pixel_shader->Release();
}
