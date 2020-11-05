#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class DomainShader
{
public:
	DomainShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system);
	~DomainShader();
private:
	ID3D11DomainShader* m_domain_shader;
	RenderSystem* m_system = nullptr;
private:
	friend class RenderSystem;
	friend class DeviceContext;
};

