#pragma once
#include "ResourceManager.h"

class StaticMeshManager: public ResourceManager
{
public:

	StaticMeshManager();
	~StaticMeshManager();
	StaticMeshPtr createStaticMeshFromFile(const wchar_t* file_path);
	StaticMeshPtr createStaticMeshFromFile(const wchar_t* file_path, bool is_flipped);
protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode);
};

