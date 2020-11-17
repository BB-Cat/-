#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class DeviceContext
{
public:
	DeviceContext(ID3D11DeviceContext* device_context, RenderSystem* system);
	~DeviceContext();


	void clearRenderTargetColor(const SwapChainPtr& swap_chain, float red, float green, float blue, float alpha);
	void clearGBufferRenderTargetColor(const SwapChainPtr& swap_chain, float red, float green, float blue, float alpha);

	void setRenderTargetDirect(const SwapChainPtr& swap_chain);
	void setRenderTargetDirect();
	void setRenderTargetDeferred();

	//input 0-2 for each respective shadow map -- 0 = close range shadowmap, 1 = medium range shadowmap, 2 = long range shadowmap
	void setRenderTargetShadowMap(const int& map_num);
	void setGBufferSRVs();

	void setVertexBuffer(const VertexBufferPtr& vertex_buffer);
	void setVertexBuffer(UINT size_vertex, ID3D11Buffer* buffer, ID3D11InputLayout* layout);
	void setIndexBuffer(const IndexBufferPtr& index_buffer);

	void drawTriangleList(UINT vertex_count, UINT start_vertex_index);
	void drawIndexedTriangleList(UINT index_count, UINT start_vertex_index, UINT start_index_location);
	void drawTriangleStrip(UINT vertex_count, UINT start_vertex_index);
	void drawControlPointPatchList(UINT index_count, UINT start_vertex_index, UINT start_index_location);

	void setViewportSize(UINT width, UINT height);
	void setVertexShader(const VertexShaderPtr& vertex_shader);
	void setGeometryShader(const GeometryShaderPtr& geometry_shader);
	void setHullShader(const HullShaderPtr& hull_shader);
	void setDomainShader(const DomainShaderPtr& domain_shader);
	void setPixelShader(const PixelShaderPtr& pixel_shader);
	//this function takes a direct ID3D11ComputeShader instead of the entire computeshaderptr since it is called from within the 
	//shader object.  this might need to be changed later.
	void setComputeShader(ID3D11ComputeShader* compute_shader, ID3D11ShaderResourceView* srv, ID3D11UnorderedAccessView* uav);
	void dispatchComputeShader(UINT x_dispatch_count, UINT y_dispatch_count, UINT z_dispatch_count);
	void copyResource(ID3D11Buffer* source, ID3D11Buffer* target);
	HRESULT mapResourceRead(ID3D11Buffer* source, D3D11_MAPPED_SUBRESOURCE* mapped_resource);
	HRESULT mapResourceWriteDiscard(ID3D11Buffer* source, D3D11_MAPPED_SUBRESOURCE* mapped_resource);
	void unmapResource(ID3D11Buffer* resource);
	//void retrieveComputeShaderOutput();

	void removeGeometryShader();
	void removeHullShader();
	void removeDomainShader();
	void removeComputeShader();

public:
	void setTextureVS(const TexturePtr& texture);
	//set the diffuse texture using a texture class pointer
	void setDiffuseTexPS(const TexturePtr& texture);
	//set the diffuse texture using a shaderresourceview directly
	void setDiffuseTexPS(ID3D11ShaderResourceView* srv);
	//set the diffuse and normal map textures simultaneously with two texture class pointers
	void setDiffuseNormalTexPS(const TexturePtr& diffusetex, const TexturePtr& normalmap);
	//set the diffuse and normal map textures using a shaderresourceview directly
	void setDiffuseNormalGlossTexPS(ID3D11ShaderResourceView* srv, ID3D11ShaderResourceView* srv2, ID3D11ShaderResourceView* srv3);
	//set the diffuse and normal map textures simultaneously with two texture class pointers
	void setDiffuseNormalGlossTexPS(const TexturePtr& diffusetex, const TexturePtr& normalmap, const TexturePtr& glossmap);
	//set the diffuse and normal map textures simultaneously with two texture class pointers
	void setDiffuseNormalGlossEnvironTexPS(const TexturePtr& diffusetex, const TexturePtr& normalmap, 
		const TexturePtr& glossmap, const TexturePtr& environmap);
	//set the displacement map for use in the domain shader with a texture class pointer
	void setDisplacementTexDS(const TexturePtr& texture, const TexturePtr& texture2 = nullptr);
	//set the displacement map for use in the geometry shader with a texture class pointer
	void setDisplacementTexGS(const TexturePtr& texture);
	//set the prepared shadow map into the pixel shader
	void setShadowMapPS(ID3D11ShaderResourceView* srv, ID3D11ShaderResourceView* srv2 = nullptr, ID3D11ShaderResourceView* srv3 = nullptr);
	//set texture3splat textures - diffuse, height and normal for each type of terrain
	void setTexture3SplatTex(const TexturePtr& dif1, const TexturePtr& height1, const TexturePtr& norm1, const TexturePtr& rough1, const TexturePtr& ao1,
		const TexturePtr& dif2, const TexturePtr& height2, const TexturePtr& norm2, const TexturePtr& rough2, const TexturePtr& ao2,
		const TexturePtr& dif3, const TexturePtr& height3, const TexturePtr& norm3, const TexturePtr& rough3, const TexturePtr& ao3,
		const TexturePtr& dif4, const TexturePtr& height4, const TexturePtr& norm4, const TexturePtr& rough4, const TexturePtr& ao4);

	//set the sampler state
	void setSamplerState(const SamplerPtr& sampler);

public:
	//constant buffer setting functions
	void setConstantWVPBufferVS(const MyConstantBufferPtr& buffer);

	void setConstantWVPBufferGS(const MyConstantBufferPtr& buffer);

	void setConstantWVPBufferDS(const MyConstantBufferPtr& buffer);

	void setConstantWVPBufferPS(const MyConstantBufferPtr& buffer);

	void setConstantWVPLightBufferVS(const MyConstantBufferPtr& buffer);

	void setConstantWVPLightBufferGS(const MyConstantBufferPtr& buffer);

	void setConstantWVPLightBufferDS(const MyConstantBufferPtr& buffer);

	void setConstantWVPLightBufferPS(const MyConstantBufferPtr& buffer);

	void setConstantBufferSceneLightingVS(const MyConstantBufferPtr& buffer);

	void setConstantBufferSceneLightingPS(const MyConstantBufferPtr& buffer);

	void setConstantBufferLightingPS(const MyConstantBufferPtr& buffer);

	void setConstantBufferTerrainColorVS(const MyConstantBufferPtr& buffer);

	void setConstantTransformBufferVS(const MyConstantBufferPtr& buffer);

	void setConstantTransformBufferGS(const MyConstantBufferPtr& buffer);

	void setConstantTransformBufferDS(const MyConstantBufferPtr& buffer);

	void setConstantMeshPropertyBufferPS(const MyConstantBufferPtr& buffer);

	void setConstantHullBufferHS(const MyConstantBufferPtr& buffer);

	void setConstantTimeBuffer(const MyConstantBufferPtr& buffer);

	void setDSHeightBuffer(const MyConstantBufferPtr& buffer);

	void setVSTessBuffer(const MyConstantBufferPtr& buffer);

	void setNoiseBufferPS(const MyConstantBufferPtr& buffer);

	void setCloudBufferPS(const MyConstantBufferPtr& buffer);

	//////////////////////////////////

public:
	void setRasterState(ID3D11RasterizerState* state);
	void setBlendState(ID3D11BlendState* state);
	void setDepthStencilState(ID3D11DepthStencilState* state);

public:
	void saveSwapChain(SwapChainPtr swap_chain);

private:
	ID3D11DeviceContext* m_device_context;
	RenderSystem* m_system = nullptr;
	SwapChainPtr p_swapchain;

private:
	friend class MyConstantBuffer;
	friend class GBuffer;
	friend class Sprite;
	friend class Texture;
	friend class RenderSystem;
};

