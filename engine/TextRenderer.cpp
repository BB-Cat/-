#include "TextRenderer.h"
#include "RenderSystem.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"

TextRenderer::TextRenderer(HWND* hwnd, SwapChainPtr swapchain)
{

    D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        mp_d2d_factory.GetAddressOf()
    );

    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(mp_dwrite_factory.GetAddressOf())
    );

    m_display_text = L"If you see this message the text hasn't been initialized.";
    m_display_text_len = (UINT32)wcslen(m_display_text);

    mp_dwrite_factory->CreateTextFormat(
        L"Gungsuh",                // Font family name.
        NULL,                       // Font collection (NULL sets it to use the system font collection).
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        72.0f,
        L"en-us",
        &mp_text_format
    );



    mp_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    mp_text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    HRESULT hr;

    D2D1_SIZE_U size = D2D1::SizeU(m_rc.right - m_rc.left, m_rc.bottom - m_rc.top);

    m_swapchain = swapchain->m_swap_chain;

    m_dxgi_dev = GraphicsEngine::get()->getRenderSystem()->m_dxgi_device;
    hr = mp_d2d_factory->CreateDevice(m_dxgi_dev.Get(), m_d2d_dev1.GetAddressOf());


    m_d2d_dev1->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, m_d2d_devcon.GetAddressOf());

    m_d2d_devcon->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::WhiteSmoke),
        mp_brushes[WHITE].GetAddressOf()
    );

    m_d2d_devcon->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Red),
        mp_brushes[RED].GetAddressOf()
    );

    m_d2d_devcon->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::LightGoldenrodYellow),
        mp_brushes[YELLOW].GetAddressOf()
    );

    m_d2d_devcon->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        mp_brushes[BLACK].GetAddressOf()
    );



    D2D1_BITMAP_PROPERTIES1 bp;
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bp.dpiX = 96.0f;
    bp.dpiY = 96.0f;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bp.colorContext = nullptr;


    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapchain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));


    Microsoft::WRL::ComPtr<IDXGISurface> buffer;
    m_swapchain->GetBuffer(0, IID_PPV_ARGS(buffer.GetAddressOf()));

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBmp;
    m_d2d_devcon->CreateBitmapFromDxgiSurface(buffer.Get(), &bp, targetBmp.GetAddressOf());

    m_d2d_devcon->SetTarget(targetBmp.Get());


}

TextRenderer::~TextRenderer()
{

}

void TextRenderer::render(const wchar_t text[], Vec2 pos, Vec2 size, int color)
{
    //set text and check the size
    m_display_text = text;
    m_display_text_len = (UINT32)wcslen(m_display_text);



    //create the binding box area which will display the text
    D2D1_RECT_F layoutRect = D2D1::RectF(
        static_cast<FLOAT>(pos.x),
        static_cast<FLOAT>(pos.y),
        static_cast<FLOAT>((pos.x + size.x)),
        static_cast<FLOAT>((pos.y + size.y))
    );

    m_d2d_devcon->BeginDraw();

    m_d2d_devcon->DrawTextW(
        m_display_text,        // The string to render.
        m_display_text_len,    // The string's length.
        mp_text_format.Get(),    // The text format.
        layoutRect,       // The region of the window where the text will be rendered.
        mp_brushes[color].Get()     // The brush color used to draw the text.
    );

    //just to confirm location of the textbox
    //m_d2d_devcon->DrawRectangle(layoutRect, mp_brushes[WHITE]);

    m_d2d_devcon->EndDraw();
}

void TextRenderer::changeFontSize(float size)
{
    mp_dwrite_factory->CreateTextFormat(
        L"Gungsuh",                // Font family name.
        NULL,                       // Font collection (NULL sets it to use the system font collection).
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        size,
        L"en-us",
        &mp_text_format
    );
}
