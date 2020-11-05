#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class GeometryShader
{
public:
	GeometryShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system);
	~GeometryShader();
private:
	ID3D11GeometryShader* m_geo_shader;
	RenderSystem* m_system = nullptr;
private:
	friend class RenderSystem;
	friend class DeviceContext;
};

