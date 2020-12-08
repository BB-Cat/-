#include "Scene06.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "ConstantBufferSystem.h"
#include "Sprite.h"
#include "AnmEnumeration.h"

//bool Scene03::m_first_time = true;

Scene06::Scene06(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(0, 2, -5));
	CameraManager::get()->setCamRot(Vector2D(0, 0));

	//m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	//メッシュとアニメーションの読み込み
	m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\CharacterRough\\lp.fbx", true, nullptr, D3D11_CULL_BACK);
	m_model->setAnimationCategory(Animation::Type::Player);
	m_model->loadAnimation(nullptr, Animation::Player::Idle, L"..\\Assets\\CharacterRough\\lp_idle.fbx", true, true, 0, false, false);
	m_model->loadAnimation(nullptr, Animation::Player::Idle2, L"..\\Assets\\CharacterRough\\lp_idle2.fbx", true, true, 0, false, false);
	m_model->loadAnimation(nullptr, Animation::Player::Idle3, L"..\\Assets\\CharacterRough\\lp_idle3.fbx", true, true, 0, false, false);

	m_model->loadAnimation(nullptr, Animation::Player::Walk, L"..\\Assets\\CharacterRough\\lp_walk.fbx");
	m_model->loadAnimation(nullptr, Animation::Player::WalkBackward, L"..\\Assets\\CharacterRough\\lp_walkback.fbx");
	m_model->loadAnimation(nullptr, Animation::Player::Run, L"..\\Assets\\CharacterRough\\lp_run.fbx");
	m_model->loadAnimation(nullptr, Animation::Player::Stop, L"..\\Assets\\CharacterRough\\lp_stop.fbx");
	m_model->loadAnimation(nullptr, Animation::Player::Jump, L"..\\Assets\\CharacterRough\\lp_jump.fbx", false, true, 0, true, 0.99f);
	m_model->loadAnimation(nullptr, Animation::Player::LandToIdle, L"..\\Assets\\CharacterRough\\lp_land.fbx", false, true);
	m_model->loadAnimation(nullptr, Animation::Player::LandHard, L"..\\Assets\\CharacterRough\\lp_land_hard.fbx", false, true);
	m_model->loadAnimation(nullptr, Animation::Player::LandToRun, L"..\\Assets\\CharacterRough\\lp_land_to_run.fbx", false, true);
	m_model->loadAnimation(nullptr, Animation::Player::Roll, L"..\\Assets\\CharacterRough\\lp_roll.fbx", false, true);
	m_model->loadAnimation(nullptr, Animation::Player::DodgeBack, L"..\\Assets\\CharacterRough\\lp_dodge_back.fbx", false, true, 0.8f);
	m_model->loadAnimation(nullptr, Animation::Player::Attack1, L"..\\Assets\\CharacterRough\\lp_slash.fbx", false, true, 0.5f);
	m_model->loadAnimation(nullptr, Animation::Player::Attack2, L"..\\Assets\\CharacterRough\\lp_kick.fbx", false, true);


	m_model->setBlendAnimation(Animation::Player::Run);
	//アニメーションの初期設定
	m_model->setAnimation(-1);

	m_model->setAnimation(-1);

	m_global_light_rotation = Vector2D(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vector3D(0.4, 0.6, 0.0);
	m_ambient_light_color = Vector3D(1.0, 1.0, 0.8);

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));
}

Scene06::~Scene06()
{

}

void Scene06::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, width, height);

	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	m_timer++;
}

void Scene06::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(250, 700));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Animation Test");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);

	ImGui::NewLine();
	ImGui::NewLine();

	if( ImGui::Button("Walk", ImVec2(200,30))) m_model->setAnimation(Animation::Player::Walk);
	if (ImGui::Button("Run", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Run);
	if (ImGui::Button("Jump", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Jump);
	if (ImGui::Button("Roll", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Roll);
	if (ImGui::Button("Land", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::LandToIdle);
	if (ImGui::Button("Attack", ImVec2(200, 30))) m_model->setAnimation(Animation::Player::Attack2);
	if (ImGui::Button("Stop", ImVec2(200, 30))) m_model->setAnimation(-1);

	//if (ImGui::SliderFloat("", &m_blend, 0.003f, 0.00f, 1.0f))
	ImGui::Text("Animation Blending");
	if(ImGui::SliderFloat("", &m_blend, 0.0f, 1.0f, nullptr, 1.0f))
	{
		m_model->setAnimation(Animation::Player::Walk);
		m_model->setBlendAnimation(Animation::Player::Run);
	}
	m_model->setBlend(m_blend);

	//VectorToArray v(&m_global_light_rotation);
	//ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	//v = VectorToArray(&m_light_color);
	//ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	//ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	//v = VectorToArray(&m_ambient_light_color);
	//ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Animation Test Popup");
		ImGui::BeginPopupModal("Animation Test Popup");

		ImGui::TextWrapped("This scene is to confirm animations.  You can test animation blending with the blend slider.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}


	ImGui::End();
}

void Scene06::shadowRenderPass(float delta)
{
}

void Scene06::mainRenderPass(float delta)
{
	//m_sky->renderMesh(delta, Vector3D(700, 700, 700), CameraManager::get()->getCamera().getTranslation(), Vector3D(0, 0, 0), Vector4D(1.0f, 1.0f, 1.0f, 1.0f), Shaders::ATMOSPHERE);
	
	m_model->setBlendAnmFrame(m_model->getActiveAnmPercent());
	m_model->renderMesh(delta, Vector3D(1, 1, 1), Vector3D(0,0,2), Vector3D(0, 180 * 0.01745f, 0), Shaders::LAMBERT_RIMLIGHT);
}