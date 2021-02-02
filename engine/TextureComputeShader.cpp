#include "TextureComputeShader.h"
#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"

#define RBGA (16)
TextureComputeShader::TextureComputeShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system, Vec2 dimensions)
	: ComputeShader(shader_byte_code, byte_code_size, system, RBGA, RBGA, nullptr, dimensions.x * dimensions.y)
{
	if (m_compute_shader == nullptr)
	{
		throw std::exception("Texture Compute Shader not compiled successfully");
	}


	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = dimensions.x;
	desc.Height = dimensions.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	m_dimensions = dimensions;

	std::vector<float> init_tex;
	init_tex.resize(m_dimensions.x * m_dimensions.y * 4);

	//initialize the texture to be pure black
	for (size_t i = 0; i + 3 < init_tex.size();)
	{
		init_tex[i] = 1.0f;
		init_tex[i + 1] = 0.0f;
		init_tex[i + 2] = 0.0f;
		init_tex[i + 3] = 1.0f;

		i += 4;
	}


	const UINT bytesPerPixel = 16;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = &init_tex[0];
	initData.SysMemPitch = m_dimensions.x * bytesPerPixel;
	initData.SysMemSlicePitch = m_dimensions.x * m_dimensions.y * bytesPerPixel;

	HRESULT hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&desc, NULL, &m_dynamictex);



}

TextureComputeShader::~TextureComputeShader()
{
}

void TextureComputeShader::setComputeShader()
{
	m_context->setComputeShader(m_compute_shader, m_input_srv, m_output_uav);
}

void TextureComputeShader::runComputeShaderNoSet()
{
	//Dispatch
	m_context->dispatchComputeShader(m_x_dispatch_count, m_y_dispatch_count, 1);

	// Disable Compute Shader
	m_context->removeComputeShader();

	//Copy result
	m_context->copyResource(m_output, m_output_cpu_readable);
	//m_context->copyResource(m_output, m_dynamictex);

	//save a pointer to the copied data for later use
	D3D11_MAPPED_SUBRESOURCE mapped_resource;

	HRESULT hr = m_system->getImmediateDeviceContext()->mapResourceRead(m_output_cpu_readable, &mapped_resource);
	m_output_data = mapped_resource.pData;
	//m_system->getImmediateDeviceContext()->updateResource(m_dynamictex, mapped_resource.pData);
	m_system->getImmediateDeviceContext()->unmapResource(m_output_cpu_readable);

	D3D11_MAPPED_SUBRESOURCE mapped_resource2;
	hr = m_system->getImmediateDeviceContext()->mapResourceWriteDiscard(m_dynamictex, &mapped_resource2);

	//DirectX11 pads mapped resource's rowpitches for some god forsaken reason, so i need to write
	//one row of pixels at a time unless there is a better method somewhere.
	//memcpy(mapped_resource2.pData, m_output_data, m_dimensions.x * m_dimensions.y * 16);

	//TEMPORARY!!!!!!
	size_t actual_pitch = m_dimensions.x * 16; //RGBA
	int resource_pitch_in_num_floats_per_row = mapped_resource2.RowPitch / 4;
	int source_num_floats_per_row = m_dimensions.x * 4;
	int target_marker = 0;
	int source_marker = 0;
	float* fTarget = reinterpret_cast<float*>(mapped_resource2.pData);
	float* fSource = reinterpret_cast<float*>(m_output_data);

	for (int i = 0; i < m_dimensions.y; i++)
	{
		memcpy(&fTarget[target_marker], &fSource[source_marker], actual_pitch);
		target_marker += resource_pitch_in_num_floats_per_row;
		source_marker += source_num_floats_per_row;
	}
	m_system->getImmediateDeviceContext()->unmapResource(m_dynamictex);

}

void TextureComputeShader::runComputeShader()
{
	//Enable Compute Shader
	m_context->setComputeShader(m_compute_shader, m_input_srv, m_output_uav);
	
	runComputeShaderNoSet();
}

ID3D11ShaderResourceView* TextureComputeShader::getTexture()
{

	ID3D11ShaderResourceView* temp = nullptr;
	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_dynamictex, NULL, &temp);
	return temp;
}
