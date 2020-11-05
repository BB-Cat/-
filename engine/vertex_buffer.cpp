//#include "vertex_buffer.h"
//
//bool VertexBuffer::load(ID3D11Device* device, void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader)
//{
//	deviceP = device;
//
//	if(m_buffer) m_buffer->Release();
//	if(m_layout) m_layout->Release();
//
//
//
//	D3D11_BUFFER_DESC buffDesc = {};
//	buffDesc.Usage = D3D11_USAGE_DEFAULT;
//	buffDesc.ByteWidth = size_vertex * size_list;
//	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	buffDesc.CPUAccessFlags = 0;
//	buffDesc.MiscFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA initData = {};
//	initData.pSysMem = list_vertices;
//
//	m_size_vertex = size_vertex;
//	m_size_list = size_list;
//
//	if (FAILED(device->CreateBuffer(&buffDesc, &initData, &m_buffer)))
//	{
//		return false;
//	}
//
//	D3D11_INPUT_ELEMENT_DESC layout[] =
//	{
//		//semantic name -semantic index -semantic format -input slot -aligned byte offset -input slot class -instance data step rate
//		{"POSITION", 0,	DXGI_FORMAT_R32G32B32_FLOAT, 0,	0,	D3D11_INPUT_PER_VERTEX_DATA, 0}
//	};
//
//	UINT layout_size = ARRAYSIZE(layout);
//	
//	if (FAILED(device->CreateInputLayout(layout, layout_size, shader_byte_code, size_byte_shader, &m_layout)))
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool VertexBuffer::release()
//{
//	m_buffer->Release();
//	m_layout->Release();
//	delete this;
//
//	return true;
//}