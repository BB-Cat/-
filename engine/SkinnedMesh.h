#pragma once
#include "Mesh.h"
#include "MyFbxManager.h"
#include <vector>
#include "Matrix4X4.h"
#include "VectorToArray.h"
#include <DirectXMath.h>
#include "Subset.h"
#include "FbxAnm.h"



class SkinnedMesh:public Mesh
{
public:
	SkinnedMesh(const wchar_t* full_path, MyFbxManager* fbx_manager, float* topology = nullptr, D3D11_CULL_MODE culltype = D3D11_CULL_BACK);
	SkinnedMesh(const wchar_t* full_path, bool is_flipped, MyFbxManager* fbx_manager, float* topology = nullptr, D3D11_CULL_MODE culltype = D3D11_CULL_BACK);
	~SkinnedMesh();

	//set the animation category to change the number of FBX animation vector spots there are available
	void setAnimationCategory(int type);
	//load an animation using enumeration to organize by type
	void loadAnimation(int type, const wchar_t* full_path, bool looping = true, bool interruptable = true, int interruptable_frame = 0);

	void setAnimation(int type);
	void renderMesh(float elapsed_time, Vector3D scale, Vector3D position, Vector3D rotation, int shader, float animation_speed = 1.0f) override;

	void ImGui_LightProperties();

	void toggleRaster() { m_raster = !m_raster; }

	void setBlend(float f) { m_blend = f; }

private:
	std::vector<MeshData> m_meshdata;  
	std::vector<FbxAnm> m_animation;

	int m_active_animation;
	int m_queued_animation;

	//test variable for checking animation blending
	float m_blend = 0;

	TexturePtr m_metal; //temporary to test metal shader
	TexturePtr m_displace; //temporary to test displacement shader

	bool m_raster; //decides whether to render mesh as solid or wireframe
	Material_Obj m_mat;

private:
	friend class MyFbxManager;
};

