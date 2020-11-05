#pragma once
#include <unordered_map>
#include <string>
#include "Prerequisites.h"
#include "Resource.h"
#include <d3d11.h>

class ResourceManager
{
public:
	ResourceManager();
	virtual ~ResourceManager();

	ResourcePtr createResourceFromFile(const wchar_t* file_path, float* topology);
	ResourcePtr createResourceFromFile(const wchar_t * file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode);
protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t * file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode = D3D11_CULL_BACK) = 0;
private:
	std::unordered_map<std::wstring, ResourcePtr> m_map_resources;
};

