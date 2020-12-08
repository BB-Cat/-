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
		Vector2D(1, 1), pos));

	m_terrain = std::dynamic_pointer_cast<TerrainManager>(t);

	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(-7.227f, 80, -8.912f));
	CameraManager::get()->setCamRot(Vector2D(0.234f, 0.774f));

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
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(250, 400));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Test Window");
	ImGui::Text("Press 1 key to");
	ImGui::Text("display the mouse");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	ImGui::DragInt("LOD", &m_toggle_HD, 0.05f, 0, 2);
	if (ImGui::Button("Toggle Wireframe", ImVec2(200, 30))) m_rast = !m_rast;

	if (ImGui::Button("Write Text File", ImVec2(200, 30))) m_terrain->outputFiles();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Texture Generator Popup");
		ImGui::BeginPopupModal("Texture Generator Popup");

		ImGui::TextWrapped("This scene creates a heightmap with textures and normals.  You can save them to a text file which is used in Dynamic terrain to load quickly");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}


	ImGui::End();
}

void Scene03::shadowRenderPass(float delta)
{
}

void Scene03::mainRenderPass(float delta)
{
	m_terrain->render(Shaders::TERRAIN_HD_TOON, 0, m_rast, m_toggle_HD);
}