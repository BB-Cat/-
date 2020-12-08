#include "Scene04.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "Terrain.h"
#include "TerrainManager.h"
#include "ConstantBufferSystem.h"
#include "Texture3D.h"
#include "DeviceContext.h"

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
	m_ambient_light_color = Vector3D(0.6f, 0.62f, 0.48f);


	m_noise.m_noise_type = Vector4D(0, 0, 0, 1);
	m_noise.m_show_rgba = Vector4D(1, 0, 0, 0);

	m_noise.m_per_amplitude = 0.15f;
	m_noise.m_per_frequency = 4.0f;
	m_noise.m_per_gain = 0.5f;
	m_noise.m_per_lacunarity = 2.0f;
	m_noise.m_per_octaves = 10;
	m_noise.m_per_cell_size = 25.0f;

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSNoiseBuffer(m_noise);

	//initial cloud property settings
	m_cloud_props.m_cloud_density = 0.25f;
	m_cloud_props.m_per_pixel_fade_threshhold = 0.0f;
	m_cloud_props.m_per_sample_fade_threshhold = 0.15f;
	m_cloud_props.m_sampling_resolution = Vector4D(8, 7, 7, 7);
	m_cloud_props.m_sampling_weight = Vector4D(0.3, 0.3, 0.2, 0.2);
	m_cloud_props.m_speed = 0.7f;
	m_cloud_props.m_move_dir = Vector3D(0.5f, 0, 0);

	m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));

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

	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);

	m_timer++;
}

void Scene04::imGuiRender()
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
	//ImGui::SetNextWindowSize(ImVec2(250, 400));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowPos(ImVec2(0, 70));

	//create the test window
	ImGui::Begin("Scene Interface", 0, ImGuiWindowFlags_AlwaysAutoResize);

	//if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);
	ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);
	ImGui::DragFloat("Bump Height", &m_bump_height, 0.001f, 0.0f, 5.0f);
	if (ImGui::Button("Toggle Wireframe", ImVec2(200, 30))) m_rast = !m_rast;
	//if (ImGui::Button("Toggle Normal", ImVec2(200, 30))) m_toggle_norm = !m_toggle_norm;

	VectorToArray v(&m_global_light_rotation);
	ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	v = VectorToArray(&m_light_color);
	ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	v = VectorToArray(&m_ambient_light_color);
	ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	//if (ImGui::Button("Update Terrain Types", ImVec2(200, 30))) m_terrain->updateTerrainTypes();
	//if (ImGui::Button("Write Text File", ImVec2(200, 30))) m_terrain->outputFiles();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Dynamic Terrain Popup");
		ImGui::BeginPopupModal("Dynamic Terrain Popup");

		ImGui::TextWrapped("This scene uses LOD and tesselation to render large amounts of terrain. The empty spaces are NOT a bug, they are because of missing files.  I will fix them later.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();
}

void Scene04::shadowRenderPass(float delta)
{
}

void Scene04::mainRenderPass(float delta)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D->getShaderResourceView());
	m_sky->renderMesh(delta, Vector3D(1100, 1100, 1100), CameraManager::get()->getCamera().getTranslation(), Vector3D(0, 0, 0), Shaders::ATMOSPHERE);

	if (m_toggle_norm) m_terrain->render(Shaders::TERRAIN_TEST, m_bump_height, m_rast, m_toggle_HD);
	else m_terrain->render(-1, m_bump_height, m_rast, m_toggle_HD);
}