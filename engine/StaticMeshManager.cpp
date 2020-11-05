#include "StaticMeshManager.h"
#include "StaticMesh.h"

StaticMeshManager::StaticMeshManager()
{
}

StaticMeshManager::~StaticMeshManager()
{
}

StaticMeshPtr StaticMeshManager::createStaticMeshFromFile(const wchar_t* file_path)
{
	return std::static_pointer_cast<StaticMesh>(createResourceFromFile(file_path, nullptr));;
}

StaticMeshPtr StaticMeshManager::createStaticMeshFromFile(const wchar_t* file_path, bool is_flipped)
{
	return std::static_pointer_cast<StaticMesh>(createResourceFromFile(file_path, is_flipped, nullptr, D3D11_CULL_BACK));
}

Resource* StaticMeshManager::createResourceFromFileConcrete(const wchar_t* file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode)
{
	StaticMesh* mesh = nullptr;
	try
	{
		mesh = new StaticMesh(file_path, is_flipped);
	}
	catch (...) {}

	return mesh;
}
