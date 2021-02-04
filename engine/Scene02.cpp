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
	m_ground->setColor(Vec3(0.6f, 0.6f, 0.6f));
	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);


	m_mesh1 = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\ShaderSphere\\sphere.fbx", true, nullptr);
	m_mesh1->setColor(Vec3(0.5f, 0.5f, 0.3f));

	m_mesh2 = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\cube.fbx", true, nullptr);
	m_mesh2->setColor(Vec3(0.8f, 0.8f, 0.5f));

	m_mesh3 = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\ShaderSphere\\torus.fbx", true, nullptr);
	m_mesh3->setColor(Vec3(0.3f, 0.4f, 0.5f));

	m_mesh4 = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\ShaderSphere\\cone.fbx", true, nullptr);
	m_mesh4->setColor(Vec3(0.3f, 0.9f, 0.5f));

	m_global_light_rotation = Vec2(1.84f, 0.0f);

	CameraManager::get()->setCamPos(Vec3(-7.973f, 4.85f, 6.749f));
	CameraManager::get()->setCamRot(Vec2(0.2834f, 2.124f));
	CameraManager::get()->setCamState(FREE);

	AppWindow::toggleDeferredPipeline(false);
	//CameraManager::get()->update(delta, width, height);
}

Scene02::~Scene02()
{
	if (m_terrain != nullptr) delete m_terrain;
}

void Scene02::update(float delta)
{
	

	Matrix4x4 cam = CameraManager::get()->getCamera();
	Vec3 right = cam.getXDirection();
	//CameraManager::get()->setCamPos(cam.getTranslation() + right * 0.05f);

	CameraManager::get()->update(delta);
	CameraManager::get()->beginLookAt(Vec3(0, 0, 0), 0);


	m_scene_light_dir = Vec3(sinf(m_timer) * 3, abs(sinf(m_timer * 0.8f)), cosf(m_timer) * 3);
	//Lighting::get()->updateSceneLight(rot, Vec3(1.0f, 1.0f, 1.0f), 0.85f, Vec3(0.2, 0.3, 0.4));

	//m_scene_light_dir = Vec3(sinf(m_global_light_rotation.x), m_global_light_rotation.y, cosf(m_global_light_rotation.x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, Vec3(1,1,1), 1.0f, Vec3(1,1,1));

	m_timer += delta;
}

void Scene02::imGuiRender()
{
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	ImGui::End();

	//ImGui::SetNextWindowSize(ImVec2(250, 400));
	//ImGui::SetNextWindowPos(ImVec2(0, 65));

	////create the test window
	//ImGui::Begin("Shadow Mapping", 0, ImGuiWindowFlags_AlwaysAutoResize);


	//VectorToArray v(&m_global_light_rotation);
	//ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f);
	//if (m_global_light_rotation.m_y < 0) m_global_light_rotation.m_y = 0;


	//ImGui::SetNextWindowPos(ImVec2(0, 680));
	//ImGui::SetNextWindowBgAlpha(0.6f);
	//ImGui::Begin("ShadowMapping", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	//
	//VectorToArray v(&m_global_light_rotation);
	//ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f);
	//if (m_global_light_rotation.y < 0) m_global_light_rotation.y = 0;
	//
	//ImGui::End();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Shadow Mapping Popup");
		ImGui::BeginPopupModal("Shadow Mapping Popup");

		ImGui::TextWrapped("Simple Shadow mapping.  This is an old scene so there are some bugs.  I will remake this when I have time.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}


}

void Scene02::shadowRenderPass(float delta)
{
	m_mesh1->renderMesh(delta, Vec3(0.5f, 0.5f, 0.5f), Vec3(3.0f, 3.5f, 3.0f), Vec3(0 * 0.01745f, 0 * 0.01745f, 0), SHADOWMAP);

	m_mesh4->renderMesh(delta, Vec3(2.0f, 2.0f, 2.0f), Vec3(-1.5f, 1, 3.5f), Vec3(0 * 0.01745f, 0 * 0.01745f, 0), SHADOWMAP);

	m_mesh2->renderMesh(delta, Vec3(1, 1, 1), Vec3(2.0f, 0.51f, -1.0f), Vec3(0 * 0.01745f, 0 * 0.01745f, 0), SHADOWMAP);

	m_mesh3->renderMesh(delta, Vec3(2.5f, 2.5f, 2.5f), Vec3(0.5f, 1, 0.0), Vec3(75 * 0.01745f, 0 * 0.01745f, 30 * 0.01745f), SHADOWMAP);

	m_mesh3->renderMesh(delta, Vec3(2.5f, 4.5f, 2.5f), Vec3(-4.5f, 1, -10.0), Vec3(75 * 0.01745f, 130 * 0.01745f, 30 * 0.01745f), SHADOWMAP);

}

void Scene02::mainRenderPass(float delta)
{
	m_sky->renderMesh(delta, Vec3(1100, 1100, 1100), CameraManager::get()->getCamera().getTranslation(), Vec3(0, 0, 0), Shaders::ATMOSPHERE);


	m_ground->renderMesh(delta, Vec3(1.0f, 1.0f, 1.0f), Vec3(0, 0, 0), Vec3(0 * 0.01745f, 0 * 0.01745f, 0), FLAT);

	m_mesh1->renderMesh(delta, Vec3(0.5f, 0.5f, 0.5f), Vec3(3.0f, 3.5f, 3.0f), Vec3(0 * 0.01745f, 0 * 0.01745f, 0), FLAT);

	m_mesh4->renderMesh(delta, Vec3(2.0f, 2.0f, 2.0f), Vec3(-1.5f, 1, 3.5), Vec3(0 * 0.01745f, 0 * 0.01745f, 0), FLAT);

	m_mesh2->renderMesh(delta, Vec3(1, 1, 1), Vec3(2.0f, 0.03f, -1.0f), Vec3(0 * 0.01745f, 0 * 0.01745f, 0), FLAT);

	m_mesh3->renderMesh(delta, Vec3(2.5f, 2.5f, 2.5f), Vec3(0.5f, 1, 0.0), Vec3(75 * 0.01745f, 0 * 0.01745f, 30 * 0.01745f), FLAT);

	m_mesh3->renderMesh(delta, Vec3(2.5f, 5.5f, 2.5f), Vec3(-4.5f, 1, -10.0), Vec3(75 * 0.01745f, 130 * 0.01745f, 30 * 0.01745f), FLAT);

}

