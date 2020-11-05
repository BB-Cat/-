#include "Scene04.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "Terrain.h"
#include "TerrainManager.h"

//bool Scene03::m_first_time = true;

Scene04::Scene04(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	CameraManager::get()->setCamState(FREE);

	Vector2D spawn(14, 14);
	CameraManager::get()->setCamPos(Vector3D(spawn.m_x * 33 + 16, 120, spawn.m_y * 33 + 16) * PRELOADED_SCALE);

	std::shared_ptr<TerrainManager> t(new TerrainManager( Vector2D(19, 19), Vector2D(39,39), spawn));
	m_terrain = std::dynamic_pointer_cast<TerrainManager>(t);

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);


	m_global_light_rotation = Vector2D(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vector3D(1.0, 1.0, 1.0);
	m_ambient_light_color = Vector3D(1.0, 1.0, 1.0);

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));
}

Scene04::~Scene04()
{

}

void Scene04::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, width, height);

	//m_global_light_rotation += 0.01f;
	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);


	m_timer++;
}

void Scene04::imGuiRender()
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
	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);
	ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);
	ImGui::DragFloat("Bump Height", &m_bump_height, 0.001f, 0.0f, 0.5f);
	if (ImGui::Button("Toggle Wireframe", ImVec2(200, 30))) m_rast = !m_rast;
	//if (ImGui::Button("Toggle Normal", ImVec2(200, 30))) m_toggle_norm = !m_toggle_norm;

	VectorToArray v(&m_global_light_rotation);
	ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	v = VectorToArray(&m_light_color);
	ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	v = VectorToArray(&m_ambient_light_color);
	ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	if (ImGui::Button("Update Terrain Types", ImVec2(200, 30))) m_terrain->updateTerrainTypes();
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

void Scene04::shadowRenderPass(float delta)
{
}

void Scene04::mainRenderPass(float delta)
{
	m_sky->renderMesh(delta, Vector3D(1100, 1100, 1100), CameraManager::get()->getCamera().getTranslation(), Vector3D(0, 0, 0), Shaders::ATMOSPHERE);

	if (m_toggle_norm) m_terrain->render(Shaders::TERRAIN_TEST, m_bump_height, m_rast, m_toggle_HD);
	else m_terrain->render(Shaders::TEXTURE_TESS_3SPLAT, m_bump_height, m_rast, m_toggle_HD);
}