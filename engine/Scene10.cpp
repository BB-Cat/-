#include "Scene10.h"
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
#include "ActorManager.h"
#include "Cube.h"


//bool Scene03::m_first_time = true;

Scene10::Scene10(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::TP);
	CameraManager::get()->setCamPos(Vector3D(0, 4, -5));

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	m_floor = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\Floor\\floor.fbx", true, nullptr, D3D11_CULL_BACK);
	//m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\CharacterRough\\lp.fbx", true, nullptr, D3D11_CULL_BACK);

	//m_model->setAnimationCategory(Animation::Type::Player);
	//m_model->loadAnimation(Animation::Player::Run, L"..\\Assets\\CharacterRough\\lp_run.fbx");
	//m_model->loadAnimation(Animation::Player::Walk, L"..\\Assets\\CharacterRough\\lp_walk.fbx");
	//m_model->loadAnimation(Animation::Player::Jump, L"..\\Assets\\CharacterRough\\lp_jump.fbx", false, false);

	//m_model->setAnimation(-1);

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

	m_global_light_rotation = Vector2D(3.521f, 0.181f);
	m_global_light_strength = 0.85f;
	m_light_color = Vector3D(1.0f, 1.0f, 0.8f);
	m_ambient_light_color = Vector3D(1.0, 0.59f, 0.38f);

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));

	//アクターマネジャーの初期化を確認
	ActorManager::get();

}

Scene10::~Scene10()
{

}

void Scene10::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, width, height);

	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSNoiseBuffer(m_noise);
	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);

	ActorManager::get()->updateFaction(L"Player", delta);

	m_timer++;
}

void Scene10::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(215, 45));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	ImGui::End();

	//ImGui::SetNextWindowSize(ImVec2(400, 200));
	//ImGui::SetNextWindowPos(ImVec2(0, 20));

	////create the test window
	//ImGui::Begin("Test Window");
	//ImGui::Text("Press 1 key to");
	//ImGui::Text("display the mouse");

	//if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	////ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);
	//ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);

	//VectorToArray v(&m_global_light_rotation);
	//ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	//v = VectorToArray(&m_light_color);
	//ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	//ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	//v = VectorToArray(&m_ambient_light_color);
	//ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	ActorManager::get()->activePlayerImGui();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Character Interface Popup");
		ImGui::BeginPopupModal("Character Interface Popup");
		ImGui::Text("WASD: move");
		ImGui::Text("Camera: control direction");
		ImGui::Text("Shift: run");
		ImGui::Text("Space: jump");
		ImGui::Text("Right Click During Run: roll");
		ImGui::TextWrapped("This scene is for testing player movement.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	//ImGui::End();
}

void Scene10::shadowRenderPass(float delta)
{
	//m_model->renderMesh(delta, Vector3D(1, 1, 1), Vector3D(0, 0, 2), Vector3D(0, 180 * 0.01745f, 0), Shaders::SHADOWMAP);
	ActorManager::get()->renderShadowsAll(delta);
}

void Scene10::mainRenderPass(float delta)
{
	Vector3D campos = CameraManager::get()->getCamera().getTranslation();
	//m_model->renderMesh(delta, Vector3D(1, 1, 1), Vector3D(0, 0, 2), Vector3D(0, 180 * 0.01745f, 0), Shaders::LAMBERT_RIMLIGHT);
	ActorManager::get()->renderFaction(L"Player", delta);
	//m_cube->render(Vector3D(3, 3, 3), Vector3D(), Vector3D(), Shaders::LAMBERT_SPECULAR, false);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D->getShaderResourceView());
	m_sky->renderMesh(delta, Vector3D(700, 700, 700), campos, Vector3D(0, 0, 0), Shaders::ATMOSPHERE, false);
	m_floor->renderMesh(delta, Vector3D(10, 10, 10), Vector3D(campos.m_x, 0, campos.m_z), Vector3D(0, 0, 0), Shaders::SIMPLE_STAGE, false);


}