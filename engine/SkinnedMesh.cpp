#include "SkinnedMesh.h"
#include "Prerequisites.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "DeviceContext.h"
#include "VertexMesh.h"
#include <vector>
#include "Blend.h"
#include "AppWindow.h"
#include "AnmEnumeration.h"

//#include "ImGui/imgui.h"
//#include "ImGui/imgui_impl_dx11.h"
//#include "ImGui/imgui_impl_win32.h"


//INSTANCING - TODO:
/*
My current method of handling input layout for shaders is highly inefficient, but due to time constraints I will
make the skinned mesh constructor read from a bool value about whether to initialize an instanced vertex buffer or a 
singular mesh.

I will simplify instancing to ONLY consist of three additional parameters - size, position and color.  this means character models
can NOT be instanced until the system is reworked unless the character models use exactly the same transformation matrixes.

The graphics engine currently saves a shader byte code and shader size ONLY for the layout ive been using.  I will change it to save
an additional shader byte code and size for the INSTANCED version, and use a bool value to determine which value the engine returns.

The skinned mesh constructor will have a bool input for instancing and call the appropriate code.  
The vertex buffer class ALSO needs to have this change made.

After all of this is implemented, the only necessary change should be to make sure the shader used during rendering is the same type
as whatever shader code was used to initialize the mesh originally.

This is an inefficient fix for a problem caused by a lack of understanding of directX earlier in the development process, and will need to be 
fixed sometime next year.
*/

SkinnedMesh::SkinnedMesh(const wchar_t* full_path, MyFbxManager* fbx_manager, float* topology, D3D11_CULL_MODE culltype):Mesh(full_path, culltype)
{
	SkinnedMesh(full_path, false, fbx_manager, topology, culltype);

}

SkinnedMesh::SkinnedMesh(const wchar_t* full_path, bool is_flipped, MyFbxManager* fbx_manager, float* topology, D3D11_CULL_MODE culltype) : Mesh(full_path, culltype)
{

	m_meshdata = fbx_manager->loadFbxMesh(full_path, topology);
	if (m_meshdata.size() == 0) throw (int)BB_ERROR::NO_MESH_DATA;

	m_metal = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\Env.png");
	m_displace = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\EarthSlime\\earthbump.jpg");

	m_raster = true;

	m_mat.m_diffuse_color = Vec3(0.6f, 0.6f, 0.6f);	
	m_mat.m_transparency = 1.0f;
	m_mat.m_metallic = 0.4f;
	m_mat.m_shininess = 12;
	m_mat.m_specular_color = Vec3(0.9f, 0.9f, 0.9f);
	m_mat.m_rim_color = Vec3(1.0f, 1.0f, 1.0f);
	m_mat.m_rim_color.m_w = 1.0f;
	m_mat.m_rim_power = 2.0f;

	//set the mesh to not animate by default
	m_active_animation = -1;
	//make sure there is no animation queued
	m_queued_animation = -1;
	//set the blended animation to inactive by default
	m_blended_animation = -1;
	//set the blend rate to 0 by default
	m_blend = 0;
	//set the blended animation frame to 0 by default
	m_blended_anm_frame = 0;
	//set the percent tick to a negative number so it is not used unless toggled from outside
	m_percent_tick = -1;
}

SkinnedMesh::~SkinnedMesh()
{
	m_animation.clear();

	for (int i = 0; i < m_meshdata.size(); i++) m_meshdata[i].m_subs.clear();
	m_meshdata.clear();

	OutputDebugString(L"Skinned Mesh was deleted");
}

void SkinnedMesh::setAnimationCategory(int type)
{
	assert("The Animation Category is recognized.", type < Animation::Type::MAX);

	switch (type)
	{
	case Animation::Type::Player:
		m_animation.resize(Animation::Player::MAX);
		break;
	case Animation::Type::Humanoid:
		m_animation.resize(Animation::Humanoid::MAX);
		break;
	}
}

void SkinnedMesh::loadAnimation(int type, const wchar_t* full_path, bool looping, bool interruptable, int interruptable_frame,
	bool idles, int idle_frame)
{
	m_animation[type] = Fbx_Anm(full_path, GraphicsEngine::get()->getSkinnedMeshManager()->getFbxManager(),
		looping, interruptable, interruptable_frame, idles, idle_frame);
}

void SkinnedMesh::loadAnimation(void* dummy, int type, const wchar_t* full_path, bool looping, 
	bool interruptable, float interruptable_percent, bool idles, float idle_percent)
{
	m_animation[type] = Fbx_Anm(full_path, GraphicsEngine::get()->getSkinnedMeshManager()->getFbxManager(),
		looping, interruptable, interruptable_percent, idles, idle_percent);
}

void SkinnedMesh::setAnimation(int type)
{
	//if there is not an active animation of the active animation is interruptable, update the active animation value
	if (m_active_animation < 0 || m_animation[m_active_animation].getIfInterruptable() || 
		m_animation[m_active_animation].getIfFinished())
	{
		if(m_active_animation >= 0) m_animation[m_active_animation].reset();

		m_active_animation = type;
		m_blended_animation = type;
		m_blend = 0;
		m_blended_anm_frame = 0;
	}
	else m_queued_animation = type;
}

void SkinnedMesh::setAnimationFrame(int frame)
{
	if (m_active_animation < 0) return;
	else m_animation[m_active_animation].setFrame(frame);
}

void SkinnedMesh::resetAnimation()
{
	if (m_active_animation < 0) return;
	else m_animation[m_active_animation].reset();
}

void SkinnedMesh::setBlendAnimation(int type)
{
	if (type >= 0 && type < m_animation.size())
	{
		m_blended_animation = type;
	}
	else m_blended_animation = -1;
}

void SkinnedMesh::setBlendAnmFrame(float percent)
{
	m_blended_anm_frame = max( min(percent, 1.0f), 0);
}

void SkinnedMesh::setAnmPercentTick(float tick)
{
	m_percent_tick = tick;
}

void SkinnedMesh::setColor(Vec3 color)
{
	m_mat.m_diffuse_color = color;
}

bool SkinnedMesh::getIfAnimInterruptable()
{
	if (m_active_animation < 0) return true;
	else return m_animation[m_active_animation].getIfInterruptable();
}

bool SkinnedMesh::getIfAnimFinished()
{
	if (m_active_animation < 0) return true;
	else return m_animation[m_active_animation].getIfFinished();
}

Fbx_Anm * SkinnedMesh::getActiveAnimation()
{
	if(m_active_animation < 0) return nullptr;

	return &m_animation[m_active_animation];
}

float SkinnedMesh::getActiveAnmPercent()
{
	if (m_active_animation < 0) return 0;
	return m_animation[m_active_animation].getPercentCompletion();
}

bool SkinnedMesh::getAnimationLengths(float& active_length, float& blend_length)
{
	if (m_active_animation < 0) active_length = 0;
	else active_length = m_animation[m_active_animation].getTotalTime();

	if (m_blended_animation < 0)
	{
		blend_length = 0;
		return false;
	}
	
	blend_length = m_animation[m_blended_animation].getTotalTime();
	return true;

}

int SkinnedMesh::getAnimationFrameCount()
{
	if (m_active_animation < 0) return -1;
	else return m_animation[m_active_animation].getTotalFrames();
}

void SkinnedMesh::triggerAnimationFinish(bool trigger)
{
	if(m_active_animation >= 0) m_animation[m_active_animation].setFinishTrigger(trigger);
}

void SkinnedMesh::renderMesh(float elapsed_time, Vec3 scale, Vec3 position, Vec3 rotation, 
	int shader, bool is_textured, float animation_speed)
{
	BlendMode::get()->SetBlend(BlendType::ALPHA);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDepthStencilState(m_depth_stencil);

	if (m_raster == true)
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_solid_rast);
	else
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_wire_rast);


	Skeleton skeleton, blend_skeleton;
	size_t number_of_bones = 0;


	//====================================================================
	//  Primary Animation Data
	//====================================================================
	//see if the active animation needs to be changed
	if (m_active_animation < 0 || (m_queued_animation >= 0 && 
		(m_animation[m_active_animation].getIfInterruptable() || m_animation[m_active_animation].getIfFinished())))
	{
		if (m_active_animation >= 0) m_animation[m_active_animation].reset();
		m_active_animation = m_queued_animation;
		m_queued_animation = -1;
	}

	//see if there is an active animation
	if (m_active_animation >= 0)
	{
		//update the animation data. if a percent tick was sent to the render function, 
		//use the update by percent function. otherwise use delta time.
		if (animation_speed)
		{
			if (m_percent_tick >= 0)m_animation[m_active_animation].updatedByPercentage(m_percent_tick * animation_speed);
			else m_animation[m_active_animation].update(elapsed_time * animation_speed);
		}

		//this is a quick fix because we are calling the render function in the shadow mapping function AND the normal render function,
		//which is causing problems for animation blending.  we need a better solution later.
		if (animation_speed != 0)
		{
			m_percent_tick = -1;
		}

		//get the current animation pose
		skeleton = m_animation[m_active_animation].getPose();
		//get the number of bones
		number_of_bones = skeleton.m_bones.size();
	}

	//====================================================================
	//  Blend Animation Data
	//====================================================================
		//see if there is an active animation
	if (number_of_bones && m_blend != 0 && m_blended_animation >= 0)
	{
		//get the current animation pose
		blend_skeleton = m_animation[m_blended_animation].getPoseAtPercent(m_blended_anm_frame);
		//get the number of bones
		size_t number_of_bones2 = blend_skeleton.m_bones.size();
		_ASSERT_EXPR(number_of_bones == number_of_bones2, L"Skeletons with a different number of bones attempted to blend!");
	}
	else blend_skeleton = skeleton;


	//--------------------------------------------------------------------

	//draw the vertices
	for (int i = 0; i < m_meshdata.size(); i++)
	{
		//set the vertices which will be drawn
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_meshdata[i].m_vertex_buffer);
		//set the index for the vertices which will be drawn
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_meshdata[i].m_index_buffer);

		GraphicsEngine::get()->getShaderManager()->setPipeline(shader);

		//load the bone transforms for this meshdata
		Matrix4x4	bone_transforms[MAXBONES];
		for (size_t k = 0; k < MAXBONES; k++) bone_transforms[k].setIdentity();

		//apply animation data to bone transforms if currently animating
		_ASSERT_EXPR(number_of_bones < MAXBONES, L"Bones exceeding the MAXBONES limit attempted to load!");
		//set the bone transformations
		for (size_t i = 0; i < number_of_bones; i++)
		{
			bone_transforms[i] = skeleton.m_bones.at(i).transform * (1.0 - m_blend) + blend_skeleton.m_bones.at(i).transform * m_blend;
		}

		for (int j = 0; j < m_meshdata[i].m_subs.size(); j++)
		{
			if (is_textured)
			{
				//set textures for the pixel shader
				GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseNormalGlossEnvironTexPS
				(
					m_meshdata[i].m_subs[j].diffuse.m_map,
					m_meshdata[i].m_subs[j].m_map_normal,
					m_meshdata[i].m_subs[j].specular.m_map,
					m_metal
				);
			}

			//set displacement map for the domain shader
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDisplacementTexDS(m_displace);


			/* TODO : implement seperate material / shader support for each different subset of the model */
			GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetObjectLightPropertyBuffer(m_mat);


			Matrix4x4 temp = applyTransformations(m_meshdata[i].m_mesh_world, scale, rotation, position);
			GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTransformationBuffer(temp, bone_transforms);

			GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetDSHeightBuffer(0.16f);

			/////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////////////


			UINT index_count;
			if (j + 1 < m_meshdata[i].m_subs.size()) index_count = m_meshdata[i].m_subs[j + 1].index_start - m_meshdata[i].m_subs[j].index_start;
			else index_count = m_meshdata[i].m_index_buffer->getSizeIndexList() - m_meshdata[i].m_subs[j].index_start;
			
			
			if (GraphicsEngine::get()->getShaderManager()->isTessActive()) GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawControlPointPatchList(index_count, 0, m_meshdata[i].m_subs[j].index_start);
			else GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(index_count, 0, m_meshdata[i].m_subs[j].index_start);
		}
	}
	
}

void SkinnedMesh::ImGui_LightProperties()
{
	//ImGui::SetNextWindowSize(ImVec2(200, 400));
	//ImGui::SetNextWindowPos(ImVec2(0, 400));

	//create the test window
	//ImGui::Begin("Mesh Light Properties");
	//if (ImGui::Button("Toggle Sky")) m_show_sky = !m_show_sky;
	//if (ImGui::Button("Toggle Raster")) m_mesh->toggleRaster();
	//ImGui::DragInt("Shader Type", &m_shader_type, 0.1f, FLAT, TEXTURE_TESS_MODEL);
	VectorToArray v(&m_mat.m_diffuse_color);
	ImGui::PushItemWidth(170);
	ImGui::ColorPicker3("Diffuse", v.setArray(), ImGuiColorEditFlags_NoInputs);

	ImGui::SameLine();

	v = VectorToArray(&m_mat.m_specular_color);
	ImGui::PushItemWidth(170);
	ImGui::ColorPicker3("Specular", v.setArray(), ImGuiColorEditFlags_NoInputs);


	//v = VectorToArray(&m_mat.m_rim_color);
	//ImGui::PushItemWidth(170);
	//ImGui::ColorPicker3("Rimlight", v.setArray(), ImGuiColorEditFlags_NoInputs);


	ImGui::DragFloat("Transparency", &m_mat.m_transparency, 0.01f, 0, 1.0);
	ImGui::DragFloat("Specular Power", &m_mat.m_shininess, 0.01f, 0, 100.0);
	ImGui::DragFloat("Rim Power", &m_mat.m_rim_power, 0.01f, 0, 100.0);
	ImGui::DragFloat("Metallicness", &m_mat.m_metallic, 0.01f, 0, 1.0);

	//ImGui::End();
}


