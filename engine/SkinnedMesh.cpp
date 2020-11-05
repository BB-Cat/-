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

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"


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
	
	std::vector<VertexMesh> vertices;
	std::vector<u_int> indices;

	m_meshdata = fbx_manager->loadFbxMesh(full_path, topology);

	m_metal = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\Env.png");
	m_displace = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\EarthSlime\\earthbump.jpg");

	m_raster = true;

	m_mat.m_diffuseColor = Vector3D(0.6f, 0.7f, 0.6f);	
	m_mat.m_d = 1.0f;
	m_mat.m_diffuseColor = Vector3D(0.7f, 0.7f, 0.7f);
	m_mat.m_metallicAmount = 0.445f;
	m_mat.m_shininess = 30;
	m_mat.m_specularColor = Vector3D(0.5, 0.2, 0.5);
	m_mat.m_rimColor = Vector3D(0.4f, 0.7f, 0.7f);
	m_mat.m_rimColor.m_w = 1.0f;
	m_mat.m_rimPower = 4.4f;

	//set the mesh to not animate by default
	m_active_animation = -1;
	//make sure there is no animation queued
	m_queued_animation = -1;
}

SkinnedMesh::~SkinnedMesh()
{
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

void SkinnedMesh::loadAnimation(int type, const wchar_t* full_path, bool looping, bool interruptable, int interruptable_frame)
{
	m_animation[type] = FbxAnm(full_path, GraphicsEngine::get()->getSkinnedMeshManager()->getFbxManager(),
		looping, interruptable, interruptable_frame);
}

void SkinnedMesh::setAnimation(int type)
{
	//if there is not an active animation of the active animation is interruptable, update the active animation value
	if (m_active_animation >= 0 && m_animation[m_active_animation].getIfInterruptable()) 
		m_active_animation = type;

	else m_queued_animation = type;
}

void SkinnedMesh::renderMesh(float elapsed_time, Vector3D scale, Vector3D position, Vector3D rotation, int shader, float animation_speed)
{
	BlendMode::get()->SetBlend(BlendType::ALPHA);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDepthStencilState(m_depth_stencil);

	if (m_raster == true)
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_solid_rast);
	else
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_wire_rast);


	//check if there is a queued animation that can be activated
	if (m_active_animation < 0 || m_queued_animation >= 0 && m_animation[m_active_animation].getIfInterruptable())
	{
		//reset the frame of the previous animation
		if (m_active_animation >= 0) m_animation[m_active_animation].reset();
		//overwrite the active animation
		m_active_animation = m_queued_animation;
		//reset the queued animation value
		m_queued_animation = -1;
	}

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

		//update animation data for the mesh if the active animation is loadable
		if (m_active_animation >= 0)
		{ 
			//update the frame data for the animation
			bool unfinished = m_animation[m_active_animation].update(elapsed_time * animation_speed);
			//m_animation[Animation::Player::Run].setFrame(m_animation[m_active_animation].getPercent());
			//get the current pose
			Skeleton skeleton = m_animation[m_active_animation].getPose();

			//Skeleton skeleton2 = m_animation[Animation::Player::Run].getPose();

			size_t number_of_bones = skeleton.m_bones.size();

			_ASSERT_EXPR(number_of_bones < MAXBONES, L"Bones exceeding the MAXBONES limit attempted to load!");
			//set the bone transformations
			for (size_t i = 0; i < number_of_bones; i++)
			{
				bone_transforms[i] = skeleton.m_bones.at(i).transform;// *(1.0 - m_blend) + skeleton2.m_bones.at(i).transform * m_blend;
			}

			if(!unfinished) m_active_animation = m_queued_animation * (m_queued_animation >= 0) + -1 * ((m_queued_animation < 0));
		}

		for (int j = 0; j < m_meshdata[i].m_subs.size(); j++)
		{
			//set textures for the pixel shader
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseNormalGlossEnvironTexPS
			(
				m_meshdata[i].m_subs[j].diffuse.m_map, 
				m_meshdata[i].m_subs[j].m_map_normal,
				m_meshdata[i].m_subs[j].specular.m_map,
				m_metal
			);

			//set displacement map for the domain shader
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDisplacementTexDS(m_displace);

			/////////////////////////////////////////////////////////////////////////////
			////TODO: this needs to be cleaned up for sure
			/////////////////////////////////////////////////////////////////////////////
			//for real model rendering it WILL be important for each subset to contain its own material object structure.
			//this should be created and maintained starting from the constructor, but for now (for imgui) we will use one structure.
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
	ImGui::SetNextWindowSize(ImVec2(200, 400));
	ImGui::SetNextWindowPos(ImVec2(0, 400));

	//create the test window
	ImGui::Begin("Mesh Light Properties");
	//if (ImGui::Button("Toggle Sky")) m_show_sky = !m_show_sky;
	//if (ImGui::Button("Toggle Raster")) m_mesh->toggleRaster();
	//ImGui::DragInt("Shader Type", &m_shader_type, 0.1f, FLAT, TEXTURE_TESS_MODEL);
	VectorToArray v(&m_mat.m_diffuseColor);
	ImGui::DragFloat3("Diffuse Color", v.setArray(), 0.01f, 0, 1.0);
	ImGui::DragFloat("Transparency", &m_mat.m_d, 0.01f, 0, 1.0);

	v = VectorToArray(&m_mat.m_specularColor);
	ImGui::DragFloat3("Specular Color", v.setArray(), 0.01f, 0, 1.0);
	ImGui::DragFloat("Specular Power", &m_mat.m_shininess, 0.01f, 0, 100.0);

	v = VectorToArray(&m_mat.m_rimColor);
	ImGui::DragFloat3("Rim Color", v.setArray(), 0.01f, 0, 1.0);
	ImGui::DragFloat("Rim Power", &m_mat.m_rimPower, 0.01f, 0, 100.0);

	ImGui::DragFloat("Metallicness", &m_mat.m_metallicAmount, 0.01f, 0, 1.0);

	//ImGui::DragInt("Shader Type", &m_shader_type, 0.1f, FLAT, TEXTURE_TESS_MODEL);
	//ImGui::DragInt("Shader Type", &m_shader_type, 0.1f, FLAT, TEXTURE_TESS_MODEL);

	//mat.m_d = 1.0f;
	//mat.m_diffuseColor = Vector3D(0.7f, 0.7f, 0.7f);
	//mat.m_metallicAmount = 0.445f;
	//mat.m_shininess = 30;
	//mat.m_specularColor = m_meshdata[i].m_subs[j].specular.m_color;
	//mat.m_rimColor = Vector3D(0.4f, 0.7f, 0.7f);
	//mat.m_rimColor.m_w = 1.0f;
	//mat.m_rimPower = 4.4f;

	ImGui::End();
}

//void SkinnedMesh::setTextureResources(int shadertype, const Subset_FBX& sub)
//{
//
//	int shader_used_textures[][6] =
//	{
//		//diffuse, normal, gloss, environment, displacement, displacement2
//		{0, 0, 0, 0, 0, 0},//FLAT,
//		{1, 0, 0, 0, 0, 0},//FLAT_TEX,
//		{0, 0, 0, 0, 0, 0},//LAMBERT,
//		{0, 0, 0, 0, 0, 0},//LAMBERT_SPECULAR,
//		{0, 0, 0, 0, 0, 0},//LAMBERT_RIMLIGHT,
//		{1, 0, 0, 0, 0, 0},//TEXTURE,
//		{1, 1, 0, 0, 0, 0},//TEXTURE_NORMAL,
//		{1, 1, 1, 0, 0, 0},//TEXTURE_NORMAL_GLOSS,
//		{1, 1, 1, 1, 0, 0},//TEXTURE_ENVIRONMENT,
//		{1, 1, 1, 0, 0, 0},//GEO_TEST,
//		{1, 1, 1, 0, 1, 0},//TEXTURE_TESS_MODEL,
//		{1, 1, 0, 0, 1, 0},//TEXTURE_TESS_TERRAIN,
//		{1, 1, 0, 0, 1, 1},//TEXTURE_TESS_FLUID_TERRAIN,
//		{1, 1, 0, 0, 1, 0},//GRASS,
//	};
//
//	if (shader_used_textures[shadertype][0] && sub.diffuse.m_map != nullptr)
//
//
//
//}
