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

	//m_tex1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\Env.png");

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
	ImGui::SetNextWindowBgAlpha(0.5f);
	////create the test window
	ImGui::Begin("Scene Selection", 0, ImGuiWindowFlags_NoDecoration);

	ImVec2 size = ImVec2(280.0f, 30.0f);
	ImGui::Text("ESC: Show/Hide Mouse");
	if (ImGui::Button("Shaders",			size)) p_manager->changeScene(SceneManager::SCENE01, true);
	if (ImGui::Button("ShadowMapping",		size)) p_manager->changeScene(SceneManager::SCENE02, true);
	if (ImGui::Button("Terrain Generator",	size)) p_manager->changeScene(SceneManager::SCENE03, true);
	if (ImGui::Button("Dynamic Terrain",	size)) p_manager->changeScene(SceneManager::SCENE04, true);
	if (ImGui::Button("Tesselation",		size)) p_manager->changeScene(SceneManager::SCENE05, true);
	if (ImGui::Button("Character Animation",size)) p_manager->changeScene(SceneManager::SCENE06, true);
	if (ImGui::Button("Noise",				size)) p_manager->changeScene(SceneManager::SCENE07, true);
	if (ImGui::Button("Volumetric Clouds",	size)) p_manager->changeScene(SceneManager::SCENE08, true);
	if (ImGui::Button("Weather Map",		size)) p_manager->changeScene(SceneManager::SCENE09, true);
	if (ImGui::Button("Character Movement",	size)) p_manager->changeScene(SceneManager::SCENE10, true);
	if (ImGui::Button("Stage Creator",		size)) p_manager->changeScene(SceneManager::SCENE11, true);
	if (ImGui::Button("Stage Example",		size)) p_manager->changeScene(SceneManager::SCENE12, true);
	if (ImGui::Button("Compute Shader",		size)) p_manager->changeScene(SceneManager::SCENE13, true);

	int a = 3;

	ImGui::End();
}

void SceneSelect::shadowRenderPass(float delta)
{
}

void SceneSelect::mainRenderPass(float delta)
{
}