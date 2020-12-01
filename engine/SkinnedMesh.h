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
	void loadAnimation(int type, const wchar_t* full_path, bool looping = true, 
		bool interruptable = true, int interruptable_frame = 0, bool idles = false, int idle_frame = 0);
	//load an animation using enumeration to organize by type using percentages instead of specific frame data
	//ボイドポインターがコンストラクターの見分けができるように入れています。
	void loadAnimation(void* dummy, int type, const wchar_t* full_path, bool looping = true,
		bool interruptable = true, float interruptable_percent = 0, bool idles = false, float idle_percent = 0);

	void setAnimation(int type);
	bool getIfAnimInterruptable();
	bool getIfAnimFinished();
	int getAnimation() { return m_active_animation; }
	//returns 0 if there is no active animation, or the percentage of completion in the animation
	float getActiveAnmPercent();
	//sends the total lengths of the active and blend animations for use in percent tick calculation.  returns false if there is no blend animation.
	bool getAnimationLengths(float& active_length, float& blend_length);

	void setBlendAnimation(int type);
	void setBlendAnmFrame(float percent);
	void setAnmPercentTick(float tick);
	void setColor(Vector3D color);

	void triggerAnimationFinish(bool trigger);

	void renderMesh(float elapsed_time, Vector3D scale, Vector3D position, Vector3D rotation, int shader, 
		bool is_textured = true, float animation_speed = 1.0f) override;

	//this function does not start a new imgui window, so you need to set the 
	//window size and position as well as end the window outside this function
	void ImGui_LightProperties();

	void toggleRaster() { m_raster = !m_raster; }

	//sets the blend value for this frame ONLY.  resets after each animation call.
	void setBlend(float f) { m_blend = f; }

private:
	std::vector<MeshData> m_meshdata;  
	std::vector<FbxAnm> m_animation;

	int m_active_animation;
	int m_queued_animation;

	//variable for animation blending
	float m_blend;
	//id of the animation which is being blended into the active animation
	int m_blended_animation;
	//frame percent of the animation which is supposed to be blended
	float m_blended_anm_frame;
	//variable used to update animations by percent for blending if it has a positive value (reset every frame)
	float m_percent_tick;

	TexturePtr m_metal; //temporary to test metal shader
	TexturePtr m_displace; //temporary to test displacement shader

	bool m_raster; //decides whether to render mesh as solid or wireframe
	Material_Obj m_mat;

private:
	friend class MyFbxManager;
};

