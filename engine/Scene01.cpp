#include "Scene01.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "Lighting.h"
#include "Terrain.h"
#include "VectorToArray.h"


Scene01::Scene01(SceneManager* sm): Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	m_ground = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\Floor\\floor.fbx", true, nullptr);

	m_mesh = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\ShaderSphere\\sphere.fbx", true, nullptr);
	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	//m_sprite = GraphicsEngine::get()->getSpriteManager()->createSpriteFromFile(L"..\\Assets\\GrassBladesTex\\sprite_0059.png");

	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(0, 7, -2));
	CameraManager::get()->setCamRot(Vector2D(0, 0));

	m_global_light_rotation = Vector2D(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vector3D(1.0,1.0,1.0);
	m_ambient_light_color = Vector3D(0.2, 0.3, 1.0);

	m_seconds = 0.0f;
	m_show_sky = true;
	m_shader_type = 0;

}

Scene01::~Scene01()
{
	if (m_particle_system != nullptr) delete m_particle_system;
}

void Scene01::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->update(delta, width, height);

	//m_global_light_rotation += 0.01f;
	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	GraphicsEngine::get()->getConstantBufferSystem()->updateHullShaderBuffer(m_tesselation);
	cb_time t;
	t.m_elapsed = delta;
	t.m_time = (m_seconds += delta);
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTimeBuffer(t);

	//if (AppWindow::getKeyTrigger(' '))  p_manager->changeScene(SceneManager::SCENE02, false);
	m_timer++;
}

void Scene01::imGuiRender()
{
//=====================================================
//  Create the scene interface window
//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(250, 400));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Shaders");
	ImGui::Text("Press 1 key to");
	ImGui::Text("display the mouse");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);

	if (ImGui::Button("Toggle Sky")) m_show_sky = !m_show_sky;
	if (ImGui::Button("Toggle Raster")) m_mesh->toggleRaster();

	ImGui::DragInt("Shader Type", &m_shader_type, 0.05f, FLAT, TRIPLANAR_TEXTURE);

	VectorToArray v(&m_global_light_rotation);
	ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	v = VectorToArray(&m_light_color);
	ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	v = VectorToArray(&m_ambient_light_color);
	ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Shader Popup");
		ImGui::BeginPopupModal("Shader Popup");

		ImGui::TextWrapped("You can check my custom shaders in this scene.  Some shaders are for heightmaps / sky / volumetric rendering so they will look strange on the model.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();


}

void Scene01::shadowRenderPass(float delta)
{
}

void Scene01::mainRenderPass(float delta)
{
	if (m_show_sky) m_sky->renderMesh(delta, Vector3D(100, 100, 100), CameraManager::get()->getCamera().getTranslation(), Vector3D(0, 0, 0), Shaders::FLAT_TEX);

	m_mesh->renderMesh(delta, Vector3D(1.0, 1.0, 1.0), Vector3D(0, 4, 5), Vector3D(0 * 0.01745f, 0 * 0.01745f, 0), m_shader_type);
}
