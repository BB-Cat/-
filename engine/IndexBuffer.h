#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class DeviceContext;

class IndexBuffer
{
public:
	IndexBuffer(void* list_indices, UINT size_list, RenderSystem* system);
	~IndexBuffer();

	UINT getSizeIndexList();
private:
	UINT m_size_list;
private:
	ID3D11Buffer* m_buffer;
	RenderSystem* m_system = nullptr;

private:
	friend class DeviceContext;
};

