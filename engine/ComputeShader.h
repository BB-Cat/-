#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Prerequisites.h"

class ComputeShader
{
public:
	ComputeShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system,
		size_t input_structure_size, size_t output_structure_size, void* data, UINT input_count);
	~ComputeShader();

	//call this to replace the input data with new data
	void mapInputData(void* newdata, size_t data_size_in_bytes);
	//call this to run the compute shader. retrieve the output data
	//by using reinterpret_cast<DATATYPE*>(mappedResource.pData);
	//data is returned in the form of a void pointer
	void* runComputeShader();
	//call this after retrieving data from the compute shader.
	void unmapCPUReadable();

	void setXDispatchCount(UINT x) { m_x_dispatch_count = x; }
	void setYDispatchCount(UINT y) { m_y_dispatch_count = y; }

private:
	UINT m_x_dispatch_count = 1;
	UINT m_y_dispatch_count = 1;

	ID3D11ComputeShader* m_compute_shader;
	RenderSystem* m_system = nullptr;
private:
	//buffer pointer to the data which the compute shader uses to calculate output
	ID3D11Buffer* m_input;
	//shader resource view created from the input buffer
	ID3D11ShaderResourceView* m_input_srv;
	//buffer pointer to the data which the compute shader outputs
	ID3D11Buffer* m_output;
	//unordered access view that allows the compute shader to write results
	ID3D11UnorderedAccessView* m_output_uav;
	//cpu readable version of the output buffer that allows us to use the data, save it, etc.
	ID3D11Buffer* m_output_cpu_readable;

private:
	friend class RenderSystem;
	friend class DeviceContext;
};