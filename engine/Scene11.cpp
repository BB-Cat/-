#include "Scene11.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "ConstantBufferSystem.h"
#include "Sprite.h"
#include "AnmEnumeration.h"
#include "Texture3D.h"
#include "DeviceContext.h"
#include "Texture.h"
#include "WorldObject.h"
#include "WorldObjectManager.h"
#include "PrefabManager.h"

#include <iostream>
#include <fstream>  



//bool Scene03::m_first_time = true;

Scene11::Scene11(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::FREE);
	CameraManager::get()->setCamPos(Vec3(0, 1.5f, -5));
	CameraManager::get()->setCamRot(Vec2(0, 0));

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	//m_floor = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\Floor\\floor.fbx", true, nullptr, D3D11_CULL_BACK);

	m_global_light_rotation = Vec2(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(0.4, 0.6, 0.0);
	m_ambient_light_color = Vec3(1.0, 1.0, 0.8);

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.1, 0.1, 0.4));

	WorldObjectManager::get()->loadSceneData("backup2.txt");
}

Scene11::~Scene11()
{

}

void Scene11::update(float delta)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, AppWindow::getMouseState(true));

	m_scene_light_dir = Vec3(sinf(m_global_light_rotation.x), m_global_light_rotation.y, cosf(m_global_light_rotation.x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSNoiseBuffer(m_noise);
	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);

	m_timer++;
}

void Scene11::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	ImGui::End();

	////create the test window
	//ImGui::Begin("Scene Settings");
	//ImGui::Text("Press 1 key to display the mouse");

	//if (ImGui::Button("Scene Select")) p_manager->changeScene(SceneManager::SCENESELECT, false);
	//ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);

	//VectorToArray v(&m_global_light_rotation);
	//ImGui::DragFloat2("Light Direction", v.setArray(), 0.02f, -6.28f, 6.28f);

	//v = VectorToArray(&m_light_color);
	//ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	//ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	//v = VectorToArray(&m_ambient_light_color);
	//ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	//GameSceneManager::get()->imGuiRender();
	WorldObjectManager::get()->imGuiRender();


	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Level Creator Popup");
		ImGui::BeginPopupModal("Level Creator Popup");
		ImGui::TextWrapped("This scene is for creating scenes out of prefabs. You can change shaders, size, material and other things.  You can save a scene and load it later too.  It is a bit confusing to use for people besides me...");
		//ImGui::NewLine();
		//ImGui::TextWrapped("I am still building an AABB heirarchy system, so the collisions in the scene are not correct yet.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	//ImGui::End();
}

void Scene11::shadowRenderPass(float delta)
{
}

void Scene11::mainRenderPass(float delta)
{
	Vec3 campos = CameraManager::get()->getCamera().getTranslation();


	if (WorldObjectManager::get()->getShowPrefabEditor() == true)
	{
		PrefabManager::get()->renderEditingPrefab();
	}
	else
	{
		m_sky->renderMesh(delta, Vec3(700, 700, 700), campos, Vec3(0, 0, 0), Shaders::ATMOSPHERE, false);
		WorldObjectManager::get()->render(delta, true);
		WorldObjectManager::get()->renderSelectedHighlight();
		WorldObjectManager::get()->renderBoundingBoxes();
	}
	//GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D->getShaderResourceView());
}