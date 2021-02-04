#include "Scene01.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "Lighting.h"
#include "Terrain.h"
#include "VectorToArray.h"
#include "DeviceContext.h"


Scene01::Scene01(SceneManager* sm): Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	m_mesh = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\ShaderSphere\\sphere.fbx", true, nullptr);
	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);

	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vec3(0, 0, 0));
	CameraManager::get()->setCamRot(Vec2(0, 0));

	m_global_light_rotation = Vec2(4.2f, 0.75f);
	m_global_light_strength = 1.0f;
	m_light_color = Vec3(1.0,1.0,0.8);
	m_ambient_light_color = Vec3(0.75f, 0.5f, 0.75f);

	m_seconds = 0.0f;
	m_show_sky = true;
	m_shader_type = 0;

	m_diffuse = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Basecolor.png");
	m_normal = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Normal.png");
	m_roughness = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Metallic.png");
	m_ao = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\Env.png");

}

Scene01::~Scene01()
{

}

void Scene01::update(float delta)
{
	CameraManager::get()->update(delta);

	//m_global_light_rotation += 0.01f;
	m_scene_light_dir = Vec3(sinf(m_global_light_rotation.x), m_global_light_rotation.y, cosf(m_global_light_rotation.x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	GraphicsEngine::get()->getConstantBufferSystem()->updateHullShaderBuffer(m_tesselation);
	cb_time t;
	t.m_elapsed = delta;
	t.m_time = (m_seconds += delta);
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTimeBuffer(t);

	m_timer++;
}

void Scene01::imGuiRender()
{
//=====================================================
//  Create the scene interface window
//-----------------------------------------------------
	//ImGui::SetNextWindowSize(ImVec2(215, 45));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(1024, 280));
	ImGui::SetNextWindowPos(ImVec2(0, 480));
	ImGui::SetNextWindowBgAlpha(0.8f);

	//create the test window
	ImGui::Begin("Lighting Options", 0, ImGuiWindowFlags_NoDecoration);

	//if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);

	//if (ImGui::Button("Toggle Sky")) m_show_sky = !m_show_sky;
	//if (ImGui::Button("Toggle Raster")) m_mesh->toggleRaster();

	//ImGui::DragInt("Shader Type", &m_shader_type, 0.05f, FLAT, TRIPLANAR_TEXTURE);

	ImGui::PushItemWidth(200);
	VectorToArray v(&m_global_light_rotation);
	ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f);

	ImGui::SameLine();

	ImGui::PushItemWidth(200);
	ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	v = VectorToArray(&m_light_color);
	//ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	ImGui::PushItemWidth(170);
	ImGui::ColorPicker3("Light Color", v.setArray(), ImGuiColorEditFlags_NoInputs);

	ImGui::SameLine();

	v = VectorToArray(&m_ambient_light_color);
	ImGui::PushItemWidth(170);
	ImGui::ColorPicker3("Ambient Color", v.setArray(), ImGuiColorEditFlags_NoInputs);

	ImGui::SameLine();

	m_mesh->ImGui_LightProperties();


	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Shader Popup");
		ImGui::BeginPopupModal("Shader Popup");

		ImGui::TextWrapped("You can see my custom shaders in this scene. Use the color pickers at the bottom to change them. I learned all shaders out of class, before we had shader lessons.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}


	ImGui::End();


}

void Scene01::shadowRenderPass(float delta)
{
}

void Scene01::mainRenderPass(float delta)
{
	if (m_show_sky) m_sky->renderMesh(delta, Vec3(100, 100, 100), CameraManager::get()->getCamera().getTranslation(), Vec3(0, 0, 0), Shaders::FLAT_TEX);

	//set textures for the pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseNormalGlossEnvironTexPS
	(
		m_diffuse,
		m_normal,
		m_roughness,
		m_ao
	);

	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3(-6, 0, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::FLAT, false);
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3(-3, 0, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::LAMBERT, false);
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3( 0, 0, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::LAMBERT_SPECULAR, false);
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3( 3, 0, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::LAMBERT_RIMLIGHT, false);
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3(6, 0, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::TOON_MODEL, false);

	
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3(-6, 3, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::TEXTURE, false);
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3(-3, 3, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::TEXTURE_NORMAL, false);
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3( 0, 3, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::TEXTURE_NORMAL_GLOSS, false);
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3(3, 3, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::TEXTURE_ENVIRONMENT, false);
	m_mesh->renderMesh(delta, Vec3(1.4, 1.4, 1.4), Vec3(6, 3, 11), Vec3(0, m_seconds * 90 * 0.01745f, 0), Shaders::TOON_TEX_MODEL, false);
	
	//m_mesh->renderMesh(delta, Vector3D(1.4, 1.4, 1.4), Vector3D( 3, 3, 10), Vector3D(0, m_seconds * 90 * 0.01745f, 0), Shaders::LAMBERT);
}
