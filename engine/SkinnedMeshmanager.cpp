#include "SkinnedMeshManager.h"
#include "SkinnedMesh.h"
#include <exception>
#include <iostream>

SkinnedMeshManager::SkinnedMeshManager()
{
	try
	{
		m_fbx_manager = new MyFbxManager();
	}
	catch (...) { throw std::exception("MyFbxManager not successfully created"); }
}

SkinnedMeshManager::~SkinnedMeshManager()
{
	if (m_fbx_manager) delete m_fbx_manager;
}

SkinnedMeshPtr SkinnedMeshManager::createSkinnedMeshFromFile(const wchar_t* file_path, float* topology, D3D11_CULL_MODE cullmode)
{
	return std::static_pointer_cast<SkinnedMesh>(createResourceFromFile(file_path, true, topology, cullmode));
}

SkinnedMeshPtr SkinnedMeshManager::createSkinnedMeshFromFile(const wchar_t* file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode)
{
	return std::static_pointer_cast<SkinnedMesh>(createResourceFromFile(file_path, is_flipped, topology, cullmode));
}

MyFbxManager* SkinnedMeshManager::getFbxManager()
{
	return m_fbx_manager;
}

Resource* SkinnedMeshManager::createResourceFromFileConcrete(const wchar_t* file_path, bool is_flipped, float* topology, D3D11_CULL_MODE cullmode)
{
	SkinnedMesh* mesh = nullptr;
	try
	{
		mesh = new SkinnedMesh(file_path, is_flipped, m_fbx_manager, topology, cullmode);
	}
	catch (int error) 
	{
		mesh = nullptr;
		OutputDebugString(L"There was an error when attempting to load the mesh:\n");
		switch (error)
		{
		case BB_ERROR::NO_MESH_DATA: 
			OutputDebugString(L"The loaded mesh file was empty.\n"); break;
		case BB_ERROR::FBX_IMPORTER_UNINITIALIZED: 
			OutputDebugString(L"The FBX Importer could not be initialized.\n"); break;
		}
	}

	return mesh;
}