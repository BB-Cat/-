#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class HullShader
{
public:
	HullShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system);
	~HullShader();
private:
	ID3D11HullShader* m_hull_shader;
	RenderSystem* m_system = nullptr;
private:
	friend class RenderSystem;
	friend class DeviceContext;
};

