#include "Texture.h"
#include <DirectXTex.h>
#include "DeviceContext.h"
#include "GraphicsEngine.h"
#include "..//Include/DirectXTK-master/Inc/WICTextureLoader.h"
//#include "..//Include/DirectXTK-master/Inc/DDSTextureLoader.h"

Texture::Texture(const wchar_t* full_path): Resource(full_path)
{

	using DirectX::CreateWICTextureFromFile;

	CreateWICTextureFromFile(GraphicsEngine::get()->getRenderSystem()->m_d3d_device,
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context,full_path, &m_texture, &m_shader_res_view);
	

	ID3D11Texture2D* tex2D;
	m_texture->QueryInterface(&tex2D);
	D3D11_TEXTURE2D_DESC m_tex_desc;
	tex2D->GetDesc(&m_tex_desc);
	m_height = m_tex_desc.Height;
	m_width = m_tex_desc.Width;

	tex2D->Release();
}


Texture::~Texture()
{
	m_shader_res_view->Release();
	m_texture->Release();
}
