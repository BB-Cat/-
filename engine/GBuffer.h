#pragma once
#include "Prerequisites.h"
#include <d3d11.h>
#include "Vector2D.h"

#define NUM_BUFFERS (6)

/*


3 -High Res Shadow Map
4 -Medium Res Shadow Map
5 -Low Shadow Map
6 -Weather Map

*/

struct TextureRenderTarget
{
	ID3D11Texture2D* texture = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
};

class GBuffer
{
public:
	GBuffer(UINT width, UINT height);
	~GBuffer();

	//void clearRenderTargets(const SwapChainPtr& swap_chain, float r, float g, float b, float a);
	void renderToSwapChain(int final_pass_shader_type, Vec2 pos, Vec2 scale, Vec2 rot);
	void renderToSwapChain(int buffer, int final_pass_shader_type, Vec2 pos, Vec2 scale, Vec2 rot);

	void setShadowMapTex();

private:
	//render target member variables
	TextureRenderTarget m_render_targets[NUM_BUFFERS];
	ID3D11DepthStencilView* m_depth_stencil_view = nullptr;
	ID3D11RasterizerState* m_rasterizer_state = nullptr;

	//rendering member variables
	ID3D11Buffer* m_buffer;
	ID3D11InputLayout* m_layout;
	ID3D11DepthStencilState* m_dss;
	ID3D11RasterizerState* m_rs;
private:
	friend class DeviceContext;
};

