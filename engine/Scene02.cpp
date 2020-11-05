#include "Scene02.h"
#include "Bullet.h"
#include "Terrain.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "ActorManager.h"
#include "AppWindow.h"
#include "TextRenderer.h"
#include "MyAudio.h"



Scene02::Scene02(SceneManager* sm) : Scene(sm)
{

	m_ground = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\Floor\\floor.fbx", true, nullptr);



	m_mesh1 = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\EarthSlime\\earthslime.fbx", true, nullptr);
	m_mesh2 = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\EarthSlime\\earthslime.fbx", true, nullptr);
	m_mesh3 = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\EarthSlime\\earthslime.fbx", true, nullptr);



	CameraManager::get()->setCamPos(Vector3D(0, 4, -10));
	CameraManager::get()->setCamState(FREE);

	m_is_first_frame = true;
	AppWindow::toggleDeferredPipeline(false);
}

Scene02::~Scene02()
{
	if (m_terrain != nullptr) delete m_terrain;
}

void Scene02::update(float delta, const float& width, const float& height)
{
	if (m_is_first_frame)
	{
		m_is_first_frame = false;
	}

	CameraManager::get()->update(delta, width, height, nullptr, nullptr);

	Lighting::get()->updateSceneLight(Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x)), Vector3D(1.0f, 1.0f, 1.0f), 0.85f, Vector3D(0.2, 0.3, 0.4));

	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, Vector3D(1,1,1), 1.0f, Vector3D(1,1,1));

	m_timer++;
}

void Scene02::imGuiRender()
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

	VectorToArray v(&m_global_light_rotation);
	ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

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

void Scene02::shadowRenderPass(float delta)
{
	m_mesh1->renderMesh(delta, Vector3D(0.5f, 0.5f, 0.5f), Vector3D(-1.5f, 0, 1.5), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), SHADOWMAP);

	m_mesh1->renderMesh(delta, Vector3D(2.0f, 2.0f, 2.0f), Vector3D(-1.5f, 6, 3.5), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), SHADOWMAP);

	m_mesh2->renderMesh(delta, Vector3D(1, 1, 1), Vector3D(0.0f, 0, 4.5), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), SHADOWMAP);

	m_mesh3->renderMesh(delta, Vector3D(0.5f, 0.5f, 0.5f), Vector3D(0.5f, 0, 0.0), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), SHADOWMAP);

}

void Scene02::mainRenderPass(float delta)
{
	m_ground->renderMesh(delta, Vector3D(1.0f, 1.0f, 1.0f), Vector3D(0, 0, 0), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), FLAT_TEX);

	m_mesh1->renderMesh(delta, Vector3D(0.5f, 0.5f, 0.5f), Vector3D(-1.5f, 0, 1.5), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), FLAT_TEX);

	m_mesh1->renderMesh(delta, Vector3D(2.0f, 2.0f, 2.0f), Vector3D(-1.5f, 6, 3.5), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), FLAT_TEX);

	m_mesh2->renderMesh(delta, Vector3D(1, 1, 1), Vector3D(0.0f, 0, 4.5), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), FLAT_TEX);

	m_mesh3->renderMesh(delta, Vector3D(0.5f, 0.5f, 0.5f), Vector3D(0.5f, 0, 0.0), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), FLAT_TEX);
}

