#include "SwapChain.h"
#include "RenderSystem.h"
#include <exception>

SwapChain::SwapChain(HWND hwnd, UINT width, UINT height, RenderSystem* system) : m_system(system)
{
	ID3D11Device* device = m_system->m_d3d_device;

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format =  DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = hwnd;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = TRUE;

	HRESULT res = m_system->m_dxgi_factory->CreateSwapChain(device, &desc, &m_swap_chain);
	if (FAILED(res))
	{
		throw std::exception("SwapChain not created successfully");
	}

	ID3D11Texture2D* buffer = NULL;
	res = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
	if (FAILED(res))
	{
		throw std::exception("SwapChain buffer not retrieved successfully");
	}

	res = device->CreateRenderTargetView(buffer, NULL, &m_rtv);

	D3D11_TEXTURE2D_DESC back_buffer_desc;
	buffer->GetDesc(&back_buffer_desc);

	buffer->Release();
	if (FAILED(res))
	{
		throw std::exception("RenderTargetView not created successfully");
	}

	D3D11_TEXTURE2D_DESC tex_desc = back_buffer_desc;
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex_desc.MipLevels = 1;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.MiscFlags = 0;
	tex_desc.ArraySize = 1;
	tex_desc.CPUAccessFlags = 0;

	device->CreateTexture2D(&tex_desc, nullptr, &buffer);

	//Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	SecureZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = tex_desc.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;

	res = device->CreateDepthStencilView(buffer, &descDSV, &m_dsv);
	buffer->Release();

	if (FAILED(res))
	{
		throw std::exception("DepthStencilView not created successfully");
	}

	D3D11_RASTERIZER_DESC rast_desc;
	rast_desc.FillMode = D3D11_FILL_SOLID;
	rast_desc.CullMode = D3D11_CULL_BACK;
	rast_desc.FrontCounterClockwise = FALSE;
	rast_desc.DepthBias = 0;
	rast_desc.DepthBiasClamp = 0.0f;
	rast_desc.SlopeScaledDepthBias = 0.0f;
	rast_desc.DepthClipEnable = true;
	rast_desc.ScissorEnable = false;
	rast_desc.MultisampleEnable = false;
	rast_desc.AntialiasedLineEnable = false;

	res = device->CreateRasterizerState(&rast_desc, &m_rs);

	if (FAILED(res))
	{
		throw std::exception("RasterizerState not created successfully");
	}
}

SwapChain::~SwapChain()
{
	m_rtv->Release();
	m_swap_chain->Release();
}

bool SwapChain::present(bool vsync)
{
	m_swap_chain->Present(vsync, NULL);

	return true;
}
