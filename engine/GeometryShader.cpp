#include "GeometryShader.h"
#include "RenderSystem.h"
#include <exception>



GeometryShader::GeometryShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system) : m_system(system)
{
	if (!SUCCEEDED(m_system->m_d3d_device->CreateGeometryShader(shader_byte_code, byte_code_size, nullptr, &m_geo_shader)))
	{
		throw std::exception("GeometryShader not created successfully");
	}
}

GeometryShader::~GeometryShader()
{
	m_geo_shader->Release();
}
