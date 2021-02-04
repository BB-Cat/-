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
#include "WorldObject.h"


//bool Scene03::m_first_time = true;

Scene10::Scene10(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::TP);
	CameraManager::get()->setCamPos(Vec3(0, 4, -5));

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
	m_cloud_props.m_move_dir = Vec3(0.5f, 0, 0);

	m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));

	m_global_light_rotation = Vec2(3.521f, 0.181f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(1.0f, 1.0f, 0.8f);
	m_ambient_light_color = Vec3(1.0, 0.59f, 0.38f);

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.1, 0.1, 0.4));

	//アクターマネジャーの初期化を確認
	ActorManager::get();

}

Scene10::~Scene10()
{

}

void Scene10::update(float delta)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta);

	m_scene_light_dir = Vec3(sinf(m_global_light_rotation.x), m_global_light_rotation.y, cosf(m_global_light_rotation.x));
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
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	ImGui::End();

	ActorManager::get()->activePlayerImGui();

	//display the controls
	ImGui::SetNextWindowPos(ImVec2(0, 67));
	//ImGui::SetNextWindowSize(ImVec2(370, 220));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Controls", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Mouse : Camera");
	ImGui::Text("WASD : Move");
	ImGui::Text("Shift : Run");
	ImGui::Text("Space : Jump");
	ImGui::Text("Right Click : Roll");
	ImGui::End();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
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
	Vec3 campos = CameraManager::get()->getCamera().getTranslation();
	//m_model->renderMesh(delta, Vector3D(1, 1, 1), Vector3D(0, 0, 2), Vector3D(0, 180 * 0.01745f, 0), Shaders::LAMBERT_RIMLIGHT);
	ActorManager::get()->renderFaction(L"Player", delta);
	//m_cube->render(Vector3D(3, 3, 3), Vector3D(), Vector3D(), Shaders::LAMBERT_SPECULAR, false);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D->getShaderResourceView());
	m_sky->renderMesh(delta, Vec3(700, 700, 700), campos, Vec3(0, 0, 0), Shaders::ATMOSPHERE, false);
	m_floor->renderMesh(delta, Vec3(10, 10, 10), Vec3(campos.x, 0, campos.z), Vec3(0, 0, 0), Shaders::SIMPLE_STAGE, false);


}