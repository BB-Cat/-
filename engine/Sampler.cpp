#include "Sampler.h"
#include "GraphicsEngine.h"

Sampler::Sampler(RenderSystem* system)
{
	D3D11_SAMPLER_DESC sample_desc = {};
	sample_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sample_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sample_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sample_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sample_desc.MipLODBias = 0;
	sample_desc.MaxAnisotropy = 16;
	sample_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sample_desc.MinLOD = 0;
	sample_desc.MaxLOD = D3D11_FLOAT32_MAX;

	//GraphicsEngine::get()->getRenderSystem()->CreateSamplerState(&sample_desc, &m_sampler);
	//system->crea
	system->m_d3d_device->CreateSamplerState(&sample_desc, &m_sampler);
}

Sampler::~Sampler()
{
}
