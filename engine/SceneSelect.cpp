#include "SceneSelect.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "Lighting.h"
#include "Terrain.h"
#include "VectorToArray.h"
#include "Texture.h"
#include "ComputeShader.h"


bool SceneSelect::m_popup_toggle = true;

SceneSelect::SceneSelect(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(8, 7, -6));

	m_tex1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\Env.png");

}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::update(float delta, const float& width, const float& height)
{
}

void SceneSelect::imGuiRender()
{
	//if (ImGui::Button("Explanation")) m_popup_toggle = true;
	//if (m_popup_toggle)
	//{
	//	ImGui::SetNextWindowSize(ImVec2(400, 400));
	//	Vector2D size = AppWindow::getScreenSize();

	//	ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));

	//	ImGui::OpenPopup("Welcome");
	//	ImGui::BeginPopupModal("Welcome");

	//	if (ImGui::Button("Okay", ImVec2(100, 30))) m_popup_toggle = false;
	//	ImGui::EndPopup();
	//}

	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(1000, 700));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Scene Selection");
	ImGui::Text("Press 1 key to display the mouse");
	if (ImGui::Button("Shaders", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE01, true);
	if (ImGui::Button("ShadowMapping", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE02, true);
	if (ImGui::Button("Terrain Generator", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE03, true);
	if (ImGui::Button("Dynamic Terrain", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE04, true);
	if (ImGui::Button("Tesselation", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE05, true);
	if (ImGui::Button("Character Animation", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE06, true);
	if (ImGui::Button("Noise", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE07, true);
	if (ImGui::Button("Volumetric Clouds", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE08, true);
	if (ImGui::Button("Weather Map", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE09, true);
	if (ImGui::Button("Character Movement", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE10, true);
	if (ImGui::Button("Stage Creator", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE11, true);
	if (ImGui::Button("Stage Example", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE12, true);
	if (ImGui::Button("Compute Shader", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE13, true);

	ImGui::End();
}

void SceneSelect::shadowRenderPass(float delta)
{
}

void SceneSelect::mainRenderPass(float delta)
{
}