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
#include "WorldObject.h"
#include "WorldObjectManager.h"
#include "ActorManager.h"

#include <iostream>
#include <fstream>  



//bool Scene03::m_first_time = true;

Scene12::Scene12(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::TP);
	CameraManager::get()->setCamPos(Vec3(0, 4, -5));

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
	m_cloud_props.m_move_dir = Vec3(0.5f, 0, 0);

	//m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));

	m_global_light_rotation = Vec2(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(0.4, 0.6, 0.0);
	m_ambient_light_color = Vec3(1.0, 1.0, 0.8);

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.1, 0.1, 0.4));

	WorldObjectManager::get()->loadSceneData("backup2.txt");

	//アクターマネジャーの初期化を確認
	ActorManager::get();
	if(ActorManager::get()->isNoPlayer()) ActorManager::get()->setPlayer(std::shared_ptr<Player>(new Player(true)));
	ActorManager::get()->setActivePlayerPosition(Vec3(0, 20.0f, 20));
}

Scene12::~Scene12()
{

}

void Scene12::update(float delta)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta);

	m_scene_light_dir = Vec3(sinf(m_global_light_rotation.x), m_global_light_rotation.y, cosf(m_global_light_rotation.x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSNoiseBuffer(m_noise);
	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);


	// TEMP!!! //////  This is terrible code and needs to be cleaned up now that the collisions are working (mostly)

	Vec3 old_ppos = ActorManager::get()->getActivePlayerPosition() + Vec3(0, 1.5f, 0);
	ActorManager::get()->updateFaction(L"Player", delta);
	Vec3 new_ppos = ActorManager::get()->getActivePlayerPosition() + Vec3(0, 1.5f, 0);

	Vec3 adjusted_pos = WorldObjectManager::get()->BBoxCollisionResolveCont(old_ppos, new_ppos, Vec3(2, 3, 2));
	Vec3 copy = adjusted_pos;

	//if (!(adjusted_pos == new_ppos))
	//{
	//	//for (int i = 0; i < 5; i++)
	//	//{
	//	//	adjusted_pos = WorldObjectManager::get()->BoundingBoxCollisionContinuous(old_ppos, adjusted_pos, Vec3(2, 3, 2));
	//	//}
	//	hit = true;
	//	if (adjusted_pos.y <= new_ppos.y) ActorManager::get()->stopActivePlayerAscent();
	//	if (adjusted_pos.y > new_ppos.y) ActorManager::get()->stopActivePlayerJump();
	//}
	ActorManager::get()->setActivePlayerPosition(adjusted_pos - Vec3(0, 1.5f, 0));

	m_timer++;
}

void Scene12::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	//ImGui::SetNextWindowSize(ImVec2(200, 70));
	//ImGui::SetNextWindowSize(ImVec2(215, 45));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	
	//ImGui::NewLine();
	//if (ImGui::Button("Show Hitboxes", ImVec2(200, 20))) m_show_hitboxes = !m_show_hitboxes;
	ImGui::End();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Level Creator Popup");
		ImGui::BeginPopupModal("Level Creator Popup");
		ImGui::TextWrapped("Here you can test the scene you made in the scene creator.");
		ImGui::NewLine();
		ImGui::TextWrapped("I am still building an AABB heirarchy system, so the collisions in the scene are NOT correct right now, but the collision code is already working for cubes, capsules, spheres and ray picking.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::SetNextWindowPos(ImVec2(0, 107));
	//ImGui::SetNextWindowSize(ImVec2(370, 220));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Controls", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Mouse : Camera");
	ImGui::Text("WASD : Move");
	ImGui::Text("Shift : Run");
	ImGui::Text("Space : Jump");
	ImGui::Text("Right Click While Moving : Roll / Backstep");
	ImGui::End();

}

void Scene12::shadowRenderPass(float delta)
{
}

void Scene12::mainRenderPass(float delta)
{
	Vec3 campos = CameraManager::get()->getCamera().getTranslation();
	m_sky->renderMesh(delta, Vec3(700, 700, 700), campos, Vec3(0, 0, 0), Shaders::ATMOSPHERE, false);
	WorldObjectManager::get()->render(delta);
	ActorManager::get()->renderFaction(L"Player", delta);

	WorldObjectManager::get()->renderBoundingBoxes(m_show_hitboxes);
}