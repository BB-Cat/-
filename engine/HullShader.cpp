#include "HullShader.h"
#include "RenderSystem.h"
#include <exception>

HullShader::HullShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system) : m_system(system)
{
	if (!SUCCEEDED(m_system->m_d3d_device->CreateHullShader(shader_byte_code, byte_code_size, nullptr, &m_hull_shader)))
	{
		throw std::exception("HullShader not created successfully");
	}
}

HullShader::~HullShader()
{
	m_hull_shader->Release();
}
