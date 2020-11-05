#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class MyConstantBuffer
{
public:
	MyConstantBuffer(void* buffer, UINT size_buffer, RenderSystem *system);
	~MyConstantBuffer();

	void update(DeviceContextPtr context, void* buffer);

private:
	ID3D11Buffer* m_buffer;
	RenderSystem* m_system = nullptr;

private:
	friend class DeviceContext;
};

