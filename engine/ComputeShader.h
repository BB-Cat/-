#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Prerequisites.h"
#include "Vector2D.h"

class ComputeShader
{
public:
	ComputeShader(const void* shader_byte_code, size_t byte_code_size, RenderSystem* system,
		size_t input_structure_size, size_t output_structure_size, void* data, UINT input_count);
	~ComputeShader();

	//call this to replace the input data with new data
	void updateInputData(void* newdata, UINT data_size_in_bytes);
	//call this to run the compute shader. 
	//data is returned in the form of a void pointer
	/*! CAUTION ! This Compute Shader class will not clear it's output data 
	automatically. you must call getOutputData, cast it to the correct type
	and then delete it otherwise this function will simply return on call*/
	virtual void runComputeShader();
	//call this after retrieving data from the compute shader.
	void unmapCPUReadable();

	void setXDispatchCount(UINT x) { m_x_dispatch_count = x; }
	void setYDispatchCount(UINT y) { m_y_dispatch_count = y; }

	//retrieve the output data by using reinterpret_cast<DATATYPE*>(mappedResource.pData);
	void* getOutputData() { return m_output_data; }

	ID3D11ShaderResourceView* createTextureSRVFromOutput(Vec2 size);

public: //!! This needs to be switched to private from public, but I'm still not sure how to handle releasing data after use from inside the class...
	//void pointer to the output data which can be used for things like creating an SRV for image data
	void* m_output_data = nullptr;
protected:
	UINT m_x_dispatch_count = 1;
	UINT m_y_dispatch_count = 1;

	ID3D11ComputeShader* m_compute_shader;
	RenderSystem* m_system = nullptr;
	DeviceContextPtr m_context = nullptr;
protected:
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