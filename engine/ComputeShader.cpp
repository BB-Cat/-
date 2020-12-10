#include "ComputeShader.h"
#include "GraphicsEngine.h"
#include "ResourceManager.h"
#include "Vector3D.h"
#include "DeviceContext.h"

ComputeShader::ComputeShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system,
							size_t input_structure_size, size_t output_structure_size, void* data, UINT input_count): m_system(system)
{

	if (!SUCCEEDED(m_system->m_d3d_device->CreateComputeShader(shader_byte_code, byte_code_size, nullptr, &m_compute_shader)))
	{
		throw std::exception("Compute Shader not compiled successfully");
	}

	///////////////////////////////////////////////////////////////////////////////////////
	//   INPUT RESOURCE CREATION														 //
	///////////////////////////////////////////////////////////////////////////////////////
	//create a buffer using the data above.
	D3D11_BUFFER_DESC constant_data_desc;
	constant_data_desc.Usage = D3D11_USAGE_DYNAMIC;
	//this byte width needs to be equal to the size of the input structure times the number of structures
	constant_data_desc.ByteWidth = input_structure_size * input_count;//sizeof(buffer_data) * num_data_structures;
	constant_data_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	constant_data_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constant_data_desc.StructureByteStride = sizeof(input_structure_size);
	constant_data_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	HRESULT hr;
	if (data != nullptr)
	{
		D3D11_SUBRESOURCE_DATA sub_data = {};
		sub_data.pSysMem = data;
		hr = m_system->m_d3d_device->CreateBuffer(&constant_data_desc, &sub_data, &m_input);
	}
	else hr = m_system->m_d3d_device->CreateBuffer(&constant_data_desc, nullptr, &m_input);

	//create an SRV using the buffer we just made.
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srv_desc.BufferEx.FirstElement = 0;
	srv_desc.BufferEx.Flags = 0;
	srv_desc.BufferEx.NumElements = input_count;

	hr = m_system->m_d3d_device->CreateShaderResourceView(m_input, &srv_desc, &m_input_srv);

	///////////////////////////////////////////////////////////////////////////////////////
	//   OUTPUT RESOURCE CREATION														 //
	///////////////////////////////////////////////////////////////////////////////////////
	/* Right now we are setting the output desc to have the same bytewidth as the input.  
		This is probably not required and should be changed depending on the compute shader
		in correlation to the specific compute shader's usage! */

	//create the output buffer
	/* HIGH CHANCE THAT THESE SETTINGS NEED TO BE CHANGED LATER */
	D3D11_BUFFER_DESC output_desc;
	output_desc.Usage = D3D11_USAGE_DEFAULT;
	//this byte width needs to be equal to the size of the output structure times the amount of input structures
	output_desc.ByteWidth = output_structure_size * input_count;
	output_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	output_desc.CPUAccessFlags = 0;
	output_desc.StructureByteStride = sizeof(output_structure_size);
	output_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = m_system->m_d3d_device->CreateBuffer(&output_desc, 0, &m_output);

	//create a system memory version of the output buffer to read data from.
	output_desc.Usage = D3D11_USAGE_STAGING;
	output_desc.BindFlags = 0;
	output_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	hr = m_system->m_d3d_device->CreateBuffer(&output_desc, 0, &m_output_cpu_readable);
	if (hr != S_OK)
	{
		hr = m_system->m_d3d_device->GetDeviceRemovedReason();
	}

	//create an unordered access view for compute shader to write results (not exactly sure what that means yet)
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Buffer.Flags = 0;
	//I'm 99% sure this value has to be the size of the output structure divided by 4 bytes... I don't know why.  No info online at all.
	uav_desc.Buffer.NumElements = input_count * ((output_structure_size) / 4);
	uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	

	hr = m_system->m_d3d_device->CreateUnorderedAccessView(m_output, &uav_desc, &m_output_uav);



}

ComputeShader::~ComputeShader()
{
	m_compute_shader->Release();
	m_input->Release();
	m_output->Release();
	m_input_srv->Release();
	m_output_cpu_readable->Release();
	m_output_uav->Release();

	m_compute_shader = nullptr;
	m_input = nullptr;
	m_output = nullptr;
	m_input_srv = nullptr;
	m_output_cpu_readable = nullptr;
	m_output_uav = nullptr;
}

void ComputeShader::updateInputData(void* newdata, UINT data_size_in_bytes)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	//d3dDeviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	m_system->getImmediateDeviceContext()->mapResourceWriteDiscard(m_input, &resource);
	memcpy(resource.pData, newdata, data_size_in_bytes);
	m_system->getImmediateDeviceContext()->unmapResource(m_input);

	//m_system->getImmediateDeviceContext()->updateResource(m_input, newdata);
}

void ComputeShader::runComputeShader()
{
	////Make sure all previous data has been released properly, otherwise we will leak
	//if (m_output_cpu_readable != nullptr) m_output_cpu_readable->Release();

	//Enable Compute Shader
	m_system->getImmediateDeviceContext()->setComputeShader(m_compute_shader, m_input_srv, m_output_uav);

	//Dispatch
	m_system->getImmediateDeviceContext()->dispatchComputeShader(m_x_dispatch_count, m_y_dispatch_count, 1);

	// Disable Compute Shader
	m_system->getImmediateDeviceContext()->removeComputeShader();

	//Copy result
	m_system->getImmediateDeviceContext()->copyResource(m_output, m_output_cpu_readable);

	//save a pointer to the copied data for later use
	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	HRESULT hr = m_system->getImmediateDeviceContext()->mapResourceRead(m_output_cpu_readable, &mapped_resource);
	m_output_data = mapped_resource.pData;
	m_system->getImmediateDeviceContext()->unmapResource(m_output_cpu_readable);
}

void ComputeShader::unmapCPUReadable()
{
	m_system->getImmediateDeviceContext()->unmapResource(m_output_cpu_readable);
}

ID3D11ShaderResourceView* ComputeShader::createTextureSRVFromOutput(Vec2 size)
{
	//if the output data hasnt been saved yet, return a nullptr 
	if (m_output_data == nullptr) return nullptr;

	//lets create a shader resource view here to store and try displaying in ImGui.
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = size.x;
	desc.Height = size.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;



	const UINT bytesPerPixel = 16;
	//UINT sliceSize = cWidth * cHeight * bytesPerPixel;

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_output_data;
	initData.SysMemPitch = size.x * bytesPerPixel;
	initData.SysMemSlicePitch = size.x * size.y * bytesPerPixel;


	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* temp;
	HRESULT hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&desc, &initData, &tex);
	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(tex, NULL, &temp);
	tex->Release();
	//unmapCPUReadable();
	return temp;
}
