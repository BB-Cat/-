#pragma once
#include "Resource.h"
#include <d3d11.h>
#include "Vector2D.h"

class Texture: public Resource
{
public:
	Texture(const wchar_t* full_path);
	~Texture();

	Vector2D getSize() { return Vector2D(m_height, m_width); }
private:
	ID3D11Resource* m_texture = nullptr;
	ID3D11ShaderResourceView *m_shader_res_view = nullptr;

	int m_height;
	int m_width;
private:
	friend class DeviceContext;
};

