#include "Scene03.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "Terrain.h"
#include "TerrainManager.h"

Scene03::Scene03(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	Vector2D pos = Vector2D(0, 0);

	std::shared_ptr<TerrainManager> t(new TerrainManager("..\\Assets\\map.bmp", "..\\Assets\\texturesplat.bmp", 
		Vector2D(2, 2), pos));
	m_terrain = std::dynamic_pointer_cast<TerrainManager>(t);

	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(33 * pos.m_x, 55, -15 + 33 * pos.m_y));

	Lighting::get()->updateSceneLight(Vector3D(0,0.9,0), Vector3D(1,1,1), 0.85f, Vector3D(0.2,0.2,0.4));

}

Scene03::~Scene03()
{

}

void Scene03::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->update(delta, width, height);

	m_timer++;
}

void Scene03::imGuiRender()
{
	//start the ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(250, 400));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	//create the test window
	ImGui::Begin("Test Window");
	ImGui::Text("Press 1 key to");
	ImGui::Text("display the mouse");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	ImGui::DragInt("LOD", &m_toggle_HD, 0.05f, 0, 2);
	if (ImGui::Button("Toggle Wireframe", ImVec2(200, 30))) m_rast = !m_rast;

	if (ImGui::Button("Write Text File", ImVec2(200, 30))) m_terrain->outputFiles();

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

void Scene03::shadowRenderPass(float delta)
{
}

void Scene03::mainRenderPass(float delta)
{
	m_terrain->render(Shaders::TERRAIN_TEST, 0, m_rast, m_toggle_HD);
}