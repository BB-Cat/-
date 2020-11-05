#include "SceneSelect.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "Lighting.h"
#include "Terrain.h"
#include "VectorToArray.h"


bool SceneSelect::m_first_time = true;

SceneSelect::SceneSelect(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(8, 7, -6));

	if (m_first_time) m_first_time = false;
}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::update(float delta, const float& width, const float& height)
{
}

void SceneSelect::imGuiRender()
{
	//start the ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(800, 700));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	//create the test window
	ImGui::Begin("Test Window");
	ImGui::Text("Press 1 key to display the mouse");
	if (ImGui::Button("Shaders", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE01, true);
	if (ImGui::Button("ShadowMapping", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE02, true);
	if (ImGui::Button("Terrain Generator", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE03, true);
	if (ImGui::Button("Dynamic Terrain", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE04, true);
	if (ImGui::Button("Tesselation", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE05, true);
	if (ImGui::Button("Character Animation", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE06, true);
	if (ImGui::Button("Noise", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE07, true);
	if (ImGui::Button("Volumetric Clouds", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE08, true);
	if (ImGui::Button("Weather Map", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENE09, true);

	ImGui::End();

	//=====================================================
	//  Create the additional interface windows
	//-----------------------------------------------------

	//=====================================================

	//assemble the data
	ImGui::Render();
	//render the draw data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void SceneSelect::shadowRenderPass(float delta)
{
}

void SceneSelect::mainRenderPass(float delta)
{
}