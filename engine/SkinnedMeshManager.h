#pragma once
#include "ResourceManager.h"
#include "MyFbxManager.h"

class SkinnedMeshManager : public ResourceManager
{
public:
	SkinnedMeshManager();
	~SkinnedMeshManager();
	SkinnedMeshPtr createSkinnedMeshFromFile(const wchar_t* file_path, float* topology, D3D11_CULL_MODE cullmode = D3D11_CULL_BACK);
	SkinnedMeshPtr createSkinnedMeshFromFile(const wchar_t* file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode = D3D11_CULL_BACK);
public:
	MyFbxManager* getFbxManager();
private:
	MyFbxManager* m_fbx_manager;
protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode = D3D11_CULL_BACK);
};