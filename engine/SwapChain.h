#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class SwapChain
{
public:
	SwapChain(HWND hwnd, UINT width, UINT height, RenderSystem* system);
	~SwapChain();

	bool present(bool vsync);

private:
	IDXGISwapChain* m_swap_chain = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;
	ID3D11RasterizerState* m_rs;
	RenderSystem* m_system = nullptr;

private:
	friend class DeviceContext;
	friend class TextRenderer;
	friend class GBuffer;
};

