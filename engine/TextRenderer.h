#pragma once

#include <dwrite.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <wrl\client.h>
#include "Prerequisites.h"
#include "Vector2D.h"

class TextRenderer
{
public:
	TextRenderer(HWND* hwnd, SwapChainPtr swapchain);
	~TextRenderer();

	//void update(const wchar_t* string);
	void render(const wchar_t text[], Vec2 pos, Vec2 size, int color);

	void changeFontSize(float size);

public:

	enum BrushColor
	{
		BLACK,
		WHITE,
		RED,
		YELLOW,
	};

private:

	Microsoft::WRL::ComPtr<IDWriteFactory> mp_dwrite_factory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> mp_text_format;

	Microsoft::WRL::ComPtr<IDWriteTextLayout> mp_text_layout;

	const wchar_t* m_display_text;
	UINT32 m_display_text_len;

	Microsoft::WRL::ComPtr<ID2D1Factory2> mp_d2d_factory;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> mp_brushes[4];

	RECT m_rc; //rectangle which contains screen size

	Microsoft::WRL::ComPtr<IDXGIDevice> m_dxgi_dev;
	Microsoft::WRL::ComPtr<ID2D1Device1> m_d2d_dev1;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_d2d_devcon;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapchain;
};
