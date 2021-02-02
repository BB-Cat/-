#include "DeviceContext.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "MyConstantBuffer.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "PixelShader.h"
#include "ComputeShader.h"
#include "Sampler.h"
#include "Texture.h"
#include <exception>
#include "RenderSystem.h"
#include "GBuffer.h"

DeviceContext::DeviceContext(ID3D11DeviceContext* device_context, RenderSystem* system) : m_system(system), m_device_context(device_context)
{
}

DeviceContext::~DeviceContext()
{
	m_device_context->Release();
}

void DeviceContext::clearRenderTargetColor(const SwapChainPtr& swap_chain,  float red, float green, float blue, float alpha)
{
	FLOAT clear_color[] = { red,green,blue,alpha };
	m_device_context->ClearRenderTargetView(swap_chain->m_rtv, clear_color);	
	m_device_context->ClearDepthStencilView(swap_chain->m_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void DeviceContext::clearState()
{
	m_device_context->ClearState();
}

void DeviceContext::clearGBufferRenderTargetColor(const SwapChainPtr& swap_chain, float red, float green, float blue, float alpha)
{
	FLOAT clear_color[] = { red,green,blue,alpha };

	TextureRenderTarget targets[] =
	{
		m_system->m_gbuffer->m_render_targets[0],
		m_system->m_gbuffer->m_render_targets[1],
		m_system->m_gbuffer->m_render_targets[2],
		m_system->m_gbuffer->m_render_targets[3],
		m_system->m_gbuffer->m_render_targets[4],
		m_system->m_gbuffer->m_render_targets[5]
	};

	for (int i = 0; i < NUM_BUFFERS; i++)
	{
		m_device_context->ClearRenderTargetView(targets[i].renderTargetView, clear_color);
	}

	m_device_context->ClearDepthStencilView(m_system->m_gbuffer->m_depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void DeviceContext::setRenderTargetDirect(const SwapChainPtr& swap_chain)
{
	m_device_context->OMSetRenderTargets(1, &swap_chain->m_rtv, swap_chain->m_dsv);
	m_device_context->RSSetState(swap_chain->m_rs);
}

void DeviceContext::setRenderTargetDirect()
{
	m_device_context->OMSetRenderTargets(1, &p_swapchain->m_rtv, p_swapchain->m_dsv);
	m_device_context->RSSetState(p_swapchain->m_rs);
}

void DeviceContext::setRenderTargetDeferred()
{
	ID3D11RenderTargetView* targets[] =
	{
		m_system->m_gbuffer->m_render_targets[0].renderTargetView,
		m_system->m_gbuffer->m_render_targets[1].renderTargetView,
		m_system->m_gbuffer->m_render_targets[2].renderTargetView,
		m_system->m_gbuffer->m_render_targets[3].renderTargetView,
	};

	m_device_context->OMSetRenderTargets(4, targets, m_system->m_gbuffer->m_depth_stencil_view);
	m_device_context->RSSetState(m_system->m_gbuffer->m_rasterizer_state);
}

void DeviceContext::setRenderTargetShadowMap(const int& map_num)
{
	ID3D11RenderTargetView* target[] =
	{
	m_system->m_gbuffer->m_render_targets[3].renderTargetView,
	m_system->m_gbuffer->m_render_targets[4].renderTargetView,
	m_system->m_gbuffer->m_render_targets[5].renderTargetView,
	};

	ID3D11RenderTargetView* targettest = target[map_num];

	m_device_context->OMSetRenderTargets(1, &target[map_num], m_system->m_gbuffer->m_depth_stencil_view);
	m_device_context->RSSetState(m_system->m_gbuffer->m_rasterizer_state);
}

void DeviceContext::setVertexBuffer(const VertexBufferPtr& vertex_buffer)
{
	//if the same vertex buffer is attempting to be set again, return
	if (m_current_vb == vertex_buffer->m_buffer) return;

	UINT stride = vertex_buffer->m_size_vertex;
	UINT offset = 0;
	m_device_context->IASetVertexBuffers(0, 1, &vertex_buffer->m_buffer, &stride, &offset);

	m_device_context->IASetInputLayout(vertex_buffer->m_layout);
	m_current_vb = vertex_buffer->m_buffer;
}

void DeviceContext::setVertexBuffer(UINT size_vertex, ID3D11Buffer* buffer, ID3D11InputLayout* layout)
{
	//if the same vertex buffer is attempting to be set again, return
	if (m_current_vb == buffer) return;

	UINT stride = size_vertex;
	UINT offset = 0;
	m_device_context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

	m_device_context->IASetInputLayout(layout);

	m_current_vb = buffer;
}

void DeviceContext::setIndexBuffer(const IndexBufferPtr& index_buffer)
{
	//if the same index buffer is attempting to be set again, return
	if (m_current_ib == index_buffer->m_buffer) return;

	m_device_context->IASetIndexBuffer(index_buffer->m_buffer, DXGI_FORMAT_R32_UINT, 0);

	m_current_ib = index_buffer->m_buffer;
}

void DeviceContext::drawTriangleList(UINT vertex_count, UINT start_vertex_index)
{
	m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_device_context->Draw(vertex_count, start_vertex_index);
}

void DeviceContext::drawIndexedTriangleList(UINT index_count, UINT start_vertex_index, UINT start_index_location)
{
	m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_device_context->DrawIndexed(index_count, start_index_location, start_vertex_index);
}

void DeviceContext::drawTriangleStrip(UINT vertex_count, UINT start_vertex_index)
{
	m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_device_context->Draw(vertex_count, start_vertex_index);
}

void DeviceContext::drawControlPointPatchList(UINT index_count, UINT start_vertex_index, UINT start_index_location)
{
	m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	m_device_context->DrawIndexed(index_count, start_index_location, start_vertex_index);
}

void DeviceContext::setViewportSize(UINT width, UINT height)
{
	D3D11_VIEWPORT vp = {};
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_device_context->RSSetViewports(1, &vp);
}

void DeviceContext::setVertexShader(const VertexShaderPtr& vertex_shader)
{
	m_device_context->VSSetShader(vertex_shader->m_vertex_shader, nullptr, 0);
}

void DeviceContext::setGeometryShader(const GeometryShaderPtr& geometry_shader)
{
	m_device_context->GSSetShader(geometry_shader->m_geo_shader, nullptr, 0);
}

void DeviceContext::setHullShader(const HullShaderPtr& hull_shader)
{
	m_device_context->HSSetShader(hull_shader->m_hull_shader, nullptr, 0);
	
}

void DeviceContext::setDomainShader(const DomainShaderPtr& domain_shader)
{
	m_device_context->DSSetShader(domain_shader->m_domain_shader, nullptr, 0);
}

void DeviceContext::setPixelShader(const PixelShaderPtr& pixel_shader)
{
	m_device_context->PSSetShader(pixel_shader->m_pixel_shader, nullptr, 0);
}

void DeviceContext::setComputeShader(ID3D11ComputeShader* compute_shader, ID3D11ShaderResourceView* srv, ID3D11UnorderedAccessView* uav)
{
	m_device_context->CSSetShader(compute_shader, nullptr, 0);
	m_device_context->CSSetShaderResources(0, 1, &srv);
	m_device_context->CSSetUnorderedAccessViews(0, 1, &uav, 0);
}

void DeviceContext::dispatchComputeShader(UINT x_dispatch_count, UINT y_dispatch_count, UINT z_dispatch_count)
{
	m_device_context->Dispatch(x_dispatch_count, y_dispatch_count, z_dispatch_count);
}

void DeviceContext::copyResource(ID3D11Buffer* source, ID3D11Buffer* target)
{
	m_device_context->CopyResource(target, source);
}

void DeviceContext::copyResource(ID3D11Buffer* source, ID3D11Texture2D* target)
{
	m_device_context->CopyResource(target, source);
}

HRESULT DeviceContext::mapResourceRead(ID3D11Buffer* source, D3D11_MAPPED_SUBRESOURCE* mapped_resource)
{
	HRESULT hr = m_device_context->Map(source, 0, D3D11_MAP_READ, 0, mapped_resource);
	return hr;
}

HRESULT DeviceContext::mapResourceWriteDiscard(ID3D11Buffer* source, D3D11_MAPPED_SUBRESOURCE* mapped_resource)
{
	HRESULT hr = m_device_context->Map(source, 0, D3D11_MAP_WRITE_DISCARD, 0, mapped_resource);
	return hr;
}

HRESULT DeviceContext::mapResourceWriteDiscard(ID3D11Texture2D* source, D3D11_MAPPED_SUBRESOURCE* mapped_resource)
{
	HRESULT hr = m_device_context->Map(source, 0, D3D11_MAP_WRITE_DISCARD, 0, mapped_resource);
	return hr;
}

void DeviceContext::unmapResource(ID3D11Buffer* resource)
{
	m_device_context->Unmap(resource, 0);
}

void DeviceContext::unmapResource(ID3D11Texture2D* resource)
{
	m_device_context->Unmap(resource, 0);
}

void DeviceContext::updateResource(ID3D11Resource* dest, void* data)
{
	m_device_context->UpdateSubresource(dest, NULL, NULL, data, NULL, NULL);
}

void DeviceContext::removeGeometryShader()
{
	m_device_context->GSSetShader(nullptr, nullptr, 0);
}

void DeviceContext::removeHullShader()
{
	m_device_context->HSSetShader(nullptr, nullptr, 0);
}

void DeviceContext::removeDomainShader()
{
	m_device_context->DSSetShader(nullptr, nullptr, 0);
}

void DeviceContext::removeComputeShader()
{
	m_device_context->CSSetShader(nullptr, nullptr, 0);
	m_device_context->CSSetShaderResources(0, 0, nullptr);
	m_device_context->CSSetUnorderedAccessViews(0, 0, nullptr, 0);
}

void DeviceContext::setTextureVS(const TexturePtr& texture)
{
	m_device_context->VSSetShaderResources(0, 1, &texture->m_shader_res_view);
}

void DeviceContext::setTextureCS(const TexturePtr& texture)
{
	m_device_context->CSSetShaderResources(0, 1, &texture->m_shader_res_view);
}

void DeviceContext::setDiffuseTexPS(const TexturePtr& texture)
{
	m_device_context->PSSetShaderResources(0, 1, &texture->m_shader_res_view);
}

void DeviceContext::setDiffuseTexPS(ID3D11ShaderResourceView* srv)
{
	m_device_context->PSSetShaderResources(0, 1, &srv);
}

void DeviceContext::setDiffuseNormalTexPS(const TexturePtr& diffusetex, const TexturePtr& normalmap)
{
	m_device_context->PSSetShaderResources(0, 1, &diffusetex->m_shader_res_view);
	m_device_context->PSSetShaderResources(1, 1, &normalmap->m_shader_res_view);
}

void DeviceContext::setDiffuseNormalGlossTexPS(ID3D11ShaderResourceView* srv, ID3D11ShaderResourceView* srv2, ID3D11ShaderResourceView* srv3)
{
	m_device_context->PSSetShaderResources(0, 1, &srv);
	m_device_context->PSSetShaderResources(1, 1, &srv2);
	m_device_context->PSSetShaderResources(2, 1, &srv3);
}

void DeviceContext::setDiffuseNormalGlossTexPS(const TexturePtr& diffusetex, const TexturePtr& normalmap, const TexturePtr& glossmap)
{
	if(diffusetex != nullptr) m_device_context->PSSetShaderResources(0, 1, &diffusetex->m_shader_res_view);
	if(normalmap != nullptr) m_device_context->PSSetShaderResources(1, 1, &normalmap->m_shader_res_view);
	if(glossmap != nullptr) m_device_context->PSSetShaderResources(2, 1, &glossmap->m_shader_res_view);
}

void DeviceContext::setDiffuseNormalGlossEnvironTexPS(const TexturePtr& diffusetex, const TexturePtr& normalmap,
	const TexturePtr& glossmap, const TexturePtr& environmap)
{
	if(diffusetex != nullptr) m_device_context->PSSetShaderResources(0, 1, &diffusetex->m_shader_res_view);
	if(normalmap != nullptr) m_device_context->PSSetShaderResources(1, 1, &normalmap->m_shader_res_view);
	if(glossmap != nullptr) m_device_context->PSSetShaderResources(2, 1, &glossmap->m_shader_res_view);
	if(environmap != nullptr) m_device_context->PSSetShaderResources(3, 1, &environmap->m_shader_res_view);
}

void DeviceContext::setDisplacementTexDS(const TexturePtr& texture, const TexturePtr& texture2)
{
	//if (texture == nullptr) return;

	m_device_context->DSSetShaderResources(0, 1, &texture->m_shader_res_view);

	if(texture2 != nullptr)
		m_device_context->DSSetShaderResources(1, 1, &texture2->m_shader_res_view);

	m_device_context->HSSetShaderResources(0, 1, &texture->m_shader_res_view);

	if (texture2 != nullptr)
		m_device_context->HSSetShaderResources(1, 1, &texture2->m_shader_res_view);
}


void DeviceContext::setDisplacementTexGS(const TexturePtr& texture)
{
	m_device_context->GSSetShaderResources(0, 1, &texture->m_shader_res_view);
}

void DeviceContext::setShadowMapPS(ID3D11ShaderResourceView* srv, ID3D11ShaderResourceView* srv2, ID3D11ShaderResourceView* srv3)
{
	m_device_context->PSSetShaderResources(6, 1, &srv);
	if(srv2 != nullptr) m_device_context->PSSetShaderResources(7, 1, &srv2);
	if(srv3 != nullptr) m_device_context->PSSetShaderResources(8, 1, &srv3);
	//m_device_context->PSSetShaderResources(15, 1, &srv);
}

void DeviceContext::setTexture3SplatTex(const TexturePtr& dif1, const TexturePtr& height1, const TexturePtr& norm1,const TexturePtr& rough1, const TexturePtr& ao1, 
									    const TexturePtr& dif2, const TexturePtr& height2, const TexturePtr& norm2,const TexturePtr& rough2, const TexturePtr& ao2, 
									    const TexturePtr& dif3, const TexturePtr& height3, const TexturePtr& norm3,const TexturePtr& rough3, const TexturePtr& ao3,
										const TexturePtr& dif4, const TexturePtr& height4, const TexturePtr& norm4,const TexturePtr& rough4, const TexturePtr& ao4)
{
	//PIXEL SHADER
	//terrain type 1
	m_device_context->PSSetShaderResources(0, 1, &dif1->m_shader_res_view);
	m_device_context->PSSetShaderResources(1, 1, &norm1->m_shader_res_view);
	//terrain type 2
	m_device_context->PSSetShaderResources(2, 1, &dif2->m_shader_res_view);
	m_device_context->PSSetShaderResources(3, 1, &norm2->m_shader_res_view);
	//terrain type 3
	m_device_context->PSSetShaderResources(4, 1, &dif3->m_shader_res_view);
	m_device_context->PSSetShaderResources(5, 1, &norm3->m_shader_res_view);
	//terrain type 4 (cliff face)
	m_device_context->PSSetShaderResources(6, 1, &dif4->m_shader_res_view);
	m_device_context->PSSetShaderResources(7, 1, &norm4->m_shader_res_view);

	//HD only terrain textures for AO and roughness
	m_device_context->PSSetShaderResources(8, 1, &rough1->m_shader_res_view);
	m_device_context->PSSetShaderResources(9, 1, &rough2->m_shader_res_view);
	m_device_context->PSSetShaderResources(10, 1, &rough3->m_shader_res_view);
	m_device_context->PSSetShaderResources(11, 1, &rough4->m_shader_res_view);

	m_device_context->PSSetShaderResources(12, 1, &ao1->m_shader_res_view);
	m_device_context->PSSetShaderResources(13, 1, &ao2->m_shader_res_view);
	m_device_context->PSSetShaderResources(14, 1, &ao3->m_shader_res_view);
	m_device_context->PSSetShaderResources(15, 1, &ao4->m_shader_res_view);

	//DOMAIN SHADER
	m_device_context->DSSetShaderResources(0, 1, &height1->m_shader_res_view);
	m_device_context->DSSetShaderResources(1, 1, &height2->m_shader_res_view);
	m_device_context->DSSetShaderResources(2, 1, &height3->m_shader_res_view);
	m_device_context->DSSetShaderResources(3, 1, &height4->m_shader_res_view);
}

void DeviceContext::setConstantWVPBufferVS(const MyConstantBufferPtr& buffer)
{
	m_device_context->VSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantWVPBufferGS(const MyConstantBufferPtr& buffer)
{
	m_device_context->GSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantWVPBufferDS(const MyConstantBufferPtr& buffer)
{
	m_device_context->DSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantWVPBufferPS(const MyConstantBufferPtr& buffer)
{
	m_device_context->PSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantWVPBufferCS(const MyConstantBufferPtr& buffer)
{
	m_device_context->CSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantWVPLightBufferVS(const MyConstantBufferPtr& buffer)
{
	m_device_context->VSSetConstantBuffers(3, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantWVPLightBufferGS(const MyConstantBufferPtr& buffer)
{
	m_device_context->GSSetConstantBuffers(3, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantWVPLightBufferDS(const MyConstantBufferPtr& buffer)
{
	m_device_context->DSSetConstantBuffers(3, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantWVPLightBufferPS(const MyConstantBufferPtr& buffer)
{
	m_device_context->PSSetConstantBuffers(3, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantTransformBufferVS(const MyConstantBufferPtr& buffer)
{
	m_device_context->VSSetConstantBuffers(1, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantTransformBufferGS(const MyConstantBufferPtr& buffer)
{
	m_device_context->GSSetConstantBuffers(1, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantTransformBufferDS(const MyConstantBufferPtr& buffer)
{
	m_device_context->DSSetConstantBuffers(1, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantMeshPropertyBufferPS(const MyConstantBufferPtr& buffer)
{
	m_device_context->PSSetConstantBuffers(1, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantHullBufferHS(const MyConstantBufferPtr& buffer)
{
	m_device_context->HSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantTimeBuffer(const MyConstantBufferPtr& buffer)
{
		m_device_context->VSSetConstantBuffers(3, 1, &buffer->m_buffer);
		m_device_context->GSSetConstantBuffers(3, 1, &buffer->m_buffer);
}

void DeviceContext::setDSHeightBuffer(const MyConstantBufferPtr& buffer)
{
	m_device_context->DSSetConstantBuffers(2, 1, &buffer->m_buffer);
}

void DeviceContext::setVSTessBuffer(const MyConstantBufferPtr& buffer)
{
	m_device_context->VSSetConstantBuffers(5, 1, &buffer->m_buffer);
}

void DeviceContext::setNoiseBufferPS(const MyConstantBufferPtr& buffer)
{
	m_device_context->PSSetConstantBuffers(5, 1, &buffer->m_buffer);
}

void DeviceContext::setNoiseBufferCS(const MyConstantBufferPtr& buffer)
{
	m_device_context->CSSetConstantBuffers(5, 1, &buffer->m_buffer);
}

void DeviceContext::setCloudBufferPS(const MyConstantBufferPtr& buffer)
{
	m_device_context->PSSetConstantBuffers(6, 1, &buffer->m_buffer);
}

void DeviceContext::setRaymarchBufferCS(const MyConstantBufferPtr& buffer)
{
	m_device_context->CSSetConstantBuffers(7, 1, &buffer->m_buffer);
}

void DeviceContext::setGBufferSRVs()
{
	ID3D11ShaderResourceView* resources[] =
	{
		m_system->m_gbuffer->m_render_targets[0].shaderResourceView,
		m_system->m_gbuffer->m_render_targets[1].shaderResourceView,
		m_system->m_gbuffer->m_render_targets[2].shaderResourceView,
		m_system->m_gbuffer->m_render_targets[3].shaderResourceView,
		m_system->m_gbuffer->m_render_targets[4].shaderResourceView,
	};

	m_device_context->PSSetShaderResources(0, 1, &resources[0]);
	m_device_context->PSSetShaderResources(1, 1, &resources[1]);
	m_device_context->PSSetShaderResources(2, 1, &resources[2]);
	m_device_context->PSSetShaderResources(3, 1, &resources[3]);
	//m_device_context->PSSetShaderResources(4, 1, &resources[4]);
}


void DeviceContext::setConstantBufferSceneLightingPS(const MyConstantBufferPtr& buffer)
{
	m_device_context->PSSetConstantBuffers(2, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantBufferSceneLightingVS(const MyConstantBufferPtr& buffer)
{
	m_device_context->VSSetConstantBuffers(2, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantBufferSceneLightingCS(const MyConstantBufferPtr& buffer)
{
	m_device_context->CSSetConstantBuffers(2, 1, &buffer->m_buffer);
}


void DeviceContext::setConstantBufferLightingPS(const MyConstantBufferPtr& buffer)
{
	m_device_context->PSSetConstantBuffers(3, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantBufferTerrainColorVS(const MyConstantBufferPtr& buffer)
{
	m_device_context->VSSetConstantBuffers(4, 1, &buffer->m_buffer);
}

void DeviceContext::setRasterState(ID3D11RasterizerState* state)
{
	m_device_context->RSSetState(state);
}

void DeviceContext::setSamplerState(const SamplerPtr& sampler)
{
	m_device_context->PSSetSamplers(0, 1, &sampler->m_sampler);
	m_device_context->PSSetSamplers(1, 1, &sampler->m_sampler);
	m_device_context->PSSetSamplers(2, 1, &sampler->m_sampler);
	m_device_context->PSSetSamplers(3, 1, &sampler->m_sampler);

	m_device_context->DSSetSamplers(0, 1, &sampler->m_sampler);
	m_device_context->DSSetSamplers(1, 1, &sampler->m_sampler);

	m_device_context->HSSetSamplers(0, 1, &sampler->m_sampler);
	m_device_context->HSSetSamplers(1, 1, &sampler->m_sampler);
}

void DeviceContext::setBlendState(ID3D11BlendState* state)
{
	m_device_context->OMSetBlendState(state, 0, 0xffffffff);
}

void DeviceContext::setDepthStencilState(ID3D11DepthStencilState* state)
{
	if (m_current_dss == state) return;

	m_device_context->OMSetDepthStencilState(state, 0);
	m_current_dss = state;
}

void DeviceContext::saveSwapChain(SwapChainPtr swap_chain)
{
	p_swapchain = swap_chain;
}
