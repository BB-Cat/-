#include "Scene05.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "Terrain.h"
#include "TerrainManager.h"
#include "ConstantBufferSystem.h"

//bool Scene03::m_first_time = true;

Scene05::Scene05(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	CameraManager::get()->setCamState(FREE);

	Vec2 spawn(0, 0);
	CameraManager::get()->setCamPos(Vec3(spawn.x * 33 + 16, 25, -5) * PRELOADED_SCALE);
	CameraManager::get()->setCamRot(Vec2(0, 0));

	std::shared_ptr<TerrainManager> t(new TerrainManager("..\\Assets\\map.bmp", "..\\Assets\\texturesplat.bmp",
		Vec2(3, 3), Vec2(0, 0)));
	m_terrain = std::dynamic_pointer_cast<TerrainManager>(t);

	//m_terrain = std::shared_ptr<Terrain>(new Terrain(Vector2D(3, 3), false));

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);


	m_global_light_rotation = Vec2(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(1.0, 1.0, 1.0);
	m_ambient_light_color = Vec3(0.5f, 0.5f, 0.5f);

	m_toggle_HD = 0;
	//m_max_tess = 3;
	m_bump_height = 2.0f;
	m_max_tess_range = 30.0f;

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.1, 0.1, 0.4));
}

Scene05::~Scene05()
{

}

void Scene05::update(float delta)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta);

	m_scene_light_dir = Vec3(sinf(m_global_light_rotation.x), m_global_light_rotation.y, cosf(m_global_light_rotation.x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);


	m_timer++;
}

void Scene05::imGuiRender()
{
	ImGui::SetNextWindowSize(ImVec2(215, 45));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	ImGui::End();

	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	//ImGui::SetNextWindowSize(ImVec2(400, 500));
	ImGui::SetNextWindowPos(ImVec2(0, 70));
	ImGui::SetNextWindowBgAlpha(0.6f);

	//create the test window
	ImGui::Begin("Tesselation", 0, ImGuiWindowFlags_AlwaysAutoResize);

	//if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);
	if (ImGui::Button("Toggle Tesselation", ImVec2(200, 30)))
	{
		if (m_toggle_HD) m_toggle_HD = 0;
		else m_toggle_HD = 1;
	}
	if (ImGui::Button("Toggle Wireframe", ImVec2(200, 30))) m_rast = !m_rast;


	//ImGui::DragFloat("Bump Height", &m_bump_height, 0.003f, 0.0f, 5.0f);


	//if (ImGui::Button("Toggle Normal", ImVec2(200, 30))) m_toggle_norm = !m_toggle_norm;

	//ImGui::DragInt("Max Tesselation", &m_max_tess, 0.05f, 1, 15);
	//ImGui::DragInt("Min Tesselation", &m_min_tess, 0.05f, 1, 15);
	ImGui::DragInt("Tesselation Range", &m_max_tess_range, 1, 1, 200);
	//ImGui::DragInt("Min Tesselation Range", &m_min_tess_range, 1, 20, 150);

	VectorToArray v(&m_global_light_rotation);
	ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	//v = VectorToArray(&m_light_color);
	//ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	//ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	//v = VectorToArray(&m_ambient_light_color);
	//ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	//if (ImGui::Button("Update Terrain Types", ImVec2(200, 30))) m_terrain->updateTerrainTypes();
	//if (ImGui::Button("Write Text File", ImVec2(200, 30))) m_terrain->outputFiles();
	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Tesselation Popup");
		ImGui::BeginPopupModal("Tesselation Popup");

		ImGui::TextWrapped("This scene shows my tesselation system.  It is used to take a low polygon model able to use an HD height map.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();

	cb_tess t;
	t.m_max_tess = m_max_tess;
	t.m_max_tess_range = m_max_tess_range;
	t.m_min_tess = m_min_tess;
	t.m_min_tess_range = m_min_tess_range;

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetVSTesselationBuffer(t);

	////=====================================================
	////  Create the additional interface windows
	////-----------------------------------------------------

	////=====================================================

	////assemble the data
	//ImGui::Render();
	////render the draw data
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Scene05::shadowRenderPass(float delta)
{
}

void Scene05::mainRenderPass(float delta)
{
	m_sky->renderMesh(delta, Vec3(700, 700, 700), CameraManager::get()->getCamera().getTranslation(), Vec3(0, 0, 0), Shaders::ATMOSPHERE);

	//m_terrain->render(Shaders::TESSDEMO, m_bump_height, m_rast, m_toggle_HD);
	m_terrain->renderInLOD(Shaders::TESSDEMO, m_bump_height, m_rast, m_toggle_HD);

	//if (m_toggle_norm) m_terrain->render(Shaders::TERRAIN_TEST, m_bump_height, m_rast, m_toggle_HD);
	//else m_terrain->render(Shaders::TEXTURE_TESS_3SPLAT, m_bump_height, m_rast, m_toggle_HD);
}