#include "Scene02.h"
//#include "Bullet.h"
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

	AppWindow::toggleDeferredPipeline(false);
}

Scene02::~Scene02()
{
	if (m_terrain != nullptr) delete m_terrain;
}

void Scene02::update(float delta, const float& width, const float& height)
{

	CameraManager::get()->update(delta, width, height);

	Lighting::get()->updateSceneLight(Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x)), Vector3D(1.0f, 1.0f, 1.0f), 0.85f, Vector3D(0.2, 0.3, 0.4));

	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, Vector3D(1,1,1), 1.0f, Vector3D(1,1,1));

	m_timer++;
}

void Scene02::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(250, 400));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Shadow Mapping");
	ImGui::Text("Press 1 key to");
	ImGui::Text("display the mouse");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);

	VectorToArray v(&m_global_light_rotation);
	ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Shadow Mapping Popup");
		ImGui::BeginPopupModal("Shadow Mapping Popup");

		ImGui::TextWrapped("Simple Shadow mapping.  This is an old scene so there are some bugs.  I will remake this when I have time.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();
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

