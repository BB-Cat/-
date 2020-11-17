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
		throw std::exception("Compute Shader not created successfully");
	}


	///////////////////////////////////////////////////////////////////////////////////////
	//   INPUT RESOURCE CREATION														 //
	///////////////////////////////////////////////////////////////////////////////////////

	//create a buffer to be used as the compute shader input.
	/* ! this may be temporary as we might create the buffer from outside the constructor and pass it
		as a parameter.  It depends on how this goes since I don't know what I'm doing. */
	
	///////////////////////////////////////////////////////////////////////////////////////
	/* temporary test data just to check if the compute shader is able to output data */
	//Vector3D buffer_data[] =
	//{
	//	{1, 0, 0},
	//	{0, 1, 0},
	//	{0, 0, 1},
	//	{1, 1, 1},
	//};
	//UINT num_data_structures = 1;
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

	D3D11_SUBRESOURCE_DATA sub_data = {};
	sub_data.pSysMem = data;
	
	HRESULT hr;
	hr = m_system->m_d3d_device->CreateBuffer(&constant_data_desc, &sub_data, &m_input);

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

	//create an unordered access view for compute shader to write results (not exactly sure what that means yet)
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Buffer.Flags = 0;
	uav_desc.Buffer.NumElements = input_count;
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
}

void ComputeShader::mapInputData(void* newdata, size_t data_size_in_bytes)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	//d3dDeviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	m_system->getImmediateDeviceContext()->mapResourceWriteDiscard(m_input, &resource);
	memcpy(resource.pData, newdata, data_size_in_bytes);
	m_system->getImmediateDeviceContext()->unmapResource(m_input);
}

void* ComputeShader::runComputeShader()
{
	// Enable Compute Shader
	//mDeviceContext->CSSetShader(mComputeShader, nullptr, 0);
	//mDeviceContext->CSSetShaderResources(0, 1, &mInputView);
	//mDeviceContext->CSSetUnorderedAccessViews(0, 1, &mOutputUAV, 0);
	m_system->getImmediateDeviceContext()->setComputeShader(m_compute_shader, m_input_srv, m_output_uav);

	//// Dispatch
	//mDeviceContext->Dispatch(1, 1, 1);
	//// Unbind the input textures from the CS for good housekeeping
	//ID3D11ShaderResourceView* nullSRV[] = { NULL };
	//mDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	//// Unbind output from compute shader
	//ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	//mDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	m_system->getImmediateDeviceContext()->dispatchComputeShader(m_x_dispatch_count, m_y_dispatch_count, 1);

	// Disable Compute Shader
	//mDeviceContext->CSSetShader(nullptr, nullptr, 0);
	m_system->getImmediateDeviceContext()->removeComputeShader();

	// Copy result
	//mDeviceContext->CopyResource(mOutputBuffer, mOutputResultBuffer);
	m_system->getImmediateDeviceContext()->copyResource(m_output, m_output_cpu_readable);


	//// Update particle system data with output from Compute Shader
	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	HRESULT hr = m_system->getImmediateDeviceContext()->mapResourceRead(m_output_cpu_readable, &mapped_resource);


	return mapped_resource.pData;
	//if (SUCCEEDED(hr))
	//{
	//	ParticleData* dataView = reinterpret_cast<ParticleData*>(mappedResource.pData);

	//	// Update particle positions and velocities
	//	mParticleSystem.UpdatePositionAndVelocity(dataView);

	//	mDeviceContext->Unmap(mOutputResultBuffer, 0);
	//}

}

void ComputeShader::unmapCPUReadable()
{
	m_system->getImmediateDeviceContext()->unmapResource(m_output_cpu_readable);
}
