#include "Scene12.h"
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
#include "Cube.h"
#include "WorldObjectManager.h"
#include "ActorManager.h"

#include <iostream>
#include <fstream>  



//bool Scene03::m_first_time = true;

Scene12::Scene12(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::TP);
	CameraManager::get()->setCamPos(Vector3D(0, 4, -5));

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	//m_floor = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\Floor\\floor.fbx", true, nullptr, D3D11_CULL_BACK);

	m_noise.m_noise_type = Vector4D(0, 0, 0, 1);
	m_noise.m_show_rgba = Vector4D(1, 0, 0, 0);

	m_noise.m_vor_amplitude = 1.0f;
	m_noise.m_vor_frequency = 4.0f;
	m_noise.m_vor_gain = 0.3f;
	m_noise.m_vor_lacunarity = 2.0f;
	m_noise.m_vor_octaves = 1;
	m_noise.m_vor_cell_size = 30.0f;

	m_noise.m_per_amplitude = 0.75f;
	m_noise.m_per_frequency = 4.0f;
	m_noise.m_per_gain = 0.5f;
	m_noise.m_per_lacunarity = 2.0f;
	m_noise.m_per_octaves = 10;
	m_noise.m_per_cell_size = 25.0f;

	//initial cloud property settings
	m_cloud_props.m_cloud_density = 0.15f;
	m_cloud_props.m_per_pixel_fade_threshhold = 0.0f;
	m_cloud_props.m_per_sample_fade_threshhold = 0.15f;
	m_cloud_props.m_sampling_resolution = Vector4D(8, 7, 7, 7);
	m_cloud_props.m_sampling_weight = Vector4D(0.3, 0.3, 0.2, 0.2);
	m_cloud_props.m_speed = 0.7f;
	m_cloud_props.m_move_dir = Vector3D(0.5f, 0, 0);

	m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));

	m_global_light_rotation = Vector2D(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vector3D(1.0, 1.0, 0.8);
	m_ambient_light_color = Vector3D(0.3, 0.3, 0.5);

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));

	WorldObjectManager::get()->loadSceneData("trialscene.txt");

	//アクターマネジャーの初期化を確認
	ActorManager::get();
	ActorManager::get()->setActivePlayerPosition(Vector3D(0, 0.3f, 20));
}

Scene12::~Scene12()
{

}

void Scene12::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, width, height);

	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSNoiseBuffer(m_noise);
	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);


	// TEMP!!! //////  This is terrible code and needs to be cleaned up now that the collisions are working (mostly)

	Vector3D old_ppos = ActorManager::get()->getActivePlayerPosition() + Vector3D(0, 1.5f, 0);
	ActorManager::get()->updateFaction(L"Player", delta);
	Vector3D new_ppos = ActorManager::get()->getActivePlayerPosition() + Vector3D(0, 1.5f, 0);

	Vector3D adjusted_pos = WorldObjectManager::get()->CubeAABBCollision(old_ppos, new_ppos, Vector3D(2, 3, 2));
	Vector3D copy = adjusted_pos;

	if (!(adjusted_pos == new_ppos))
	{
		for (int i = 0; i < 5; i++)
		{
			adjusted_pos = WorldObjectManager::get()->CubeAABBCollision(old_ppos, adjusted_pos, Vector3D(2, 3, 2));
		}
		hit = true;
		if (adjusted_pos.m_y <= new_ppos.m_y) ActorManager::get()->stopActivePlayerAscent();
		if (adjusted_pos.m_y > new_ppos.m_y) ActorManager::get()->stopActivePlayerJump();
	}
	if (ActorManager::get()->getActivePlayerState() != PlayerState::Jump &&
		adjusted_pos - Vector3D(0, 0.5f, 0) == WorldObjectManager::get()->CubeAABBCollision
		(old_ppos, adjusted_pos - Vector3D(0, 0.5f, 0), Vector3D(2, 3, 2)))
		ActorManager::get()->startActivePlayerFall();
	ActorManager::get()->setActivePlayerPosition(adjusted_pos - Vector3D(0, 1.5f, 0));

	m_timer++;
}

void Scene12::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(200, 70));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Scene Settings");
	ImGui::Text("Press 1 key to display the mouse");

	if (ImGui::Button("Scene Select")) p_manager->changeScene(SceneManager::SCENESELECT, false);
	ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);

	//VectorToArray v(&m_global_light_rotation);
	//ImGui::DragFloat2("Light Direction", v.setArray(), 0.02f, -6.28f, 6.28f);

	//v = VectorToArray(&m_light_color);
	//ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	//ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	//v = VectorToArray(&m_ambient_light_color);
	//ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	
	//WorldObjectManager::get()->imGuiRender();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Level Example Popup");
		ImGui::BeginPopupModal("Level Example Popup");
		ImGui::TextWrapped("This is an example scene made with the creator.  It is loaded and initialized automatically and has collisions with the player character.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();
}

void Scene12::shadowRenderPass(float delta)
{
}

void Scene12::mainRenderPass(float delta)
{
	Vector3D campos = CameraManager::get()->getCamera().getTranslation();

	WorldObjectManager::get()->render();
	ActorManager::get()->renderFaction(L"Player", delta);

	//if (hit)
	//{
	//	Vector3D pos = ActorManager::get()->getActivePlayerPosition();
	//	CubePtr temp = std::shared_ptr<Cube>(PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr,
	//		Vector3D(1, 1, 1), pos, Vector3D(), nullptr));

	//	temp->render();
	//	hit = false;
	//}
	//Vector3D pos = ActorManager::get()->getActivePlayerPosition() + Vector3D(0, 0.5f, 0);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D->getShaderResourceView());
	m_sky->renderMesh(delta, Vector3D(700, 700, 700), campos, Vector3D(0, 0, 0), Shaders::WEATHER_ATMOSPHERE, false);
}