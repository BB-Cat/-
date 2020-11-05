#include "DomainShader.h"
#include "RenderSystem.h"
#include <exception>

DomainShader::DomainShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system) : m_system(system)
{
	if (!SUCCEEDED(m_system->m_d3d_device->CreateDomainShader(shader_byte_code, byte_code_size, nullptr, &m_domain_shader)))
	{
		throw std::exception("DomainShader not created successfully");
	}
}

DomainShader::~DomainShader()
{
	m_domain_shader->Release();
}
