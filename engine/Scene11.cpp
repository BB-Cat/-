#include "Scene11.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "ConstantBufferSystem.h"
#include "Sprite.h"
#include "AnmEnumeration.h"
#include "Texture3D.h"
#include "DeviceContext.h"


//bool Scene03::m_first_time = true;

Scene11::Scene11(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::FREE);
	CameraManager::get()->setCamPos(Vector3D(0, 4, -5));

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	m_floor = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\Floor\\floor.fbx", true, nullptr, D3D11_CULL_BACK);

	m_noise.m_noise_type = Vector4D(0, 0, 0, 1);
	m_noise.m_show_rgba = Vector4D(1, 0, 0, 0);

	m_noise.m_vor_amplitude = 1.0f;
	m_noise.m_vor_frequency = 4.0f;
	m_noise.m_vor_gain = 0.3f;
	m_noise.m_vor_lacunarity = 2.0f;
	m_noise.m_vor_octaves = 1;
	m_noise.m_vor_cell_size = 30.0f;

	m_noise.m_per_amplitude = 0.75f;
	m_noise.m_per_frequency = 4.0f;
	m_noise.m_per_gain = 0.5f;
	m_noise.m_per_lacunarity = 2.0f;
	m_noise.m_per_octaves = 10;
	m_noise.m_per_cell_size = 25.0f;

	//initial cloud property settings
	m_cloud_props.m_cloud_density = 0.15f;
	m_cloud_props.m_per_pixel_fade_threshhold = 0.0f;
	m_cloud_props.m_per_sample_fade_threshhold = 0.15f;
	m_cloud_props.m_sampling_resolution = Vector4D(8, 7, 7, 7);
	m_cloud_props.m_sampling_weight = Vector4D(0.3, 0.3, 0.2, 0.2);
	m_cloud_props.m_speed = 0.7f;
	m_cloud_props.m_move_dir = Vector3D(0.5f, 0, 0);

	m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));

	m_global_light_rotation = Vector2D(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vector3D(0.4, 0.6, 0.0);
	m_ambient_light_color = Vector3D(1.0, 1.0, 0.8);

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));

	//temp
	//m_cube = std::shared_ptr<Cube>(new Cube());

	//m_cubes.push_back(PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr, 
	//	Vector3D(3, 1, 1), Vector3D(0, 0, 0), Vector3D(0, 0, 0), nullptr));
}

Scene11::~Scene11()
{

}

void Scene11::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, width, height);

	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSNoiseBuffer(m_noise);
	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);

	m_timer++;
}

void Scene11::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(400, 200));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Test Window");
	ImGui::Text("Press 1 key to");
	ImGui::Text("display the mouse");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);
	ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);

	VectorToArray v(&m_global_light_rotation);
	ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	v = VectorToArray(&m_light_color);
	ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	v = VectorToArray(&m_ambient_light_color);
	ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	if (ImGui::Button("Spawn Cube"))
	{
		m_show_window = true;
		m_spawn_pos = Vector3D(0, 0, 0);
		m_spawn_scale = Vector3D(1, 1, 1);
		m_spawn_rot = Vector3D(0, 0, 0);

		m_cubes.push_back(PrimitiveGenerator::get()->createCube(nullptr, nullptr, nullptr, m_spawn_scale, m_spawn_pos, m_spawn_rot, nullptr));
	}
	v = VectorToArray(&m_spawn_pos);
	ImGui::DragFloat3("Position", v.setArray(), 0.05f, -100.0f, 100.0f);
	v = VectorToArray(&m_spawn_scale);
	ImGui::DragFloat3("Scale", v.setArray(), 0.05f, -100.0f, 100.0f);
	v = VectorToArray(&m_spawn_rot);
	ImGui::DragFloat3("Rot", v.setArray(), 0.01f, -100.0f, 100.0f);

	if (m_cubes.size())
	{
		m_cubes[m_cubes.size() - 1]->setPosition(m_spawn_pos);
		m_cubes[m_cubes.size() - 1]->setScale(m_spawn_scale);
		m_cubes[m_cubes.size() - 1]->setRotation(m_spawn_rot);
	}
	//TOMORROW IMPLEMENT THIS
	//if (m_show_window)
	//{
	//	ImGui::SetNextWindowSize(ImVec2(100, 300));
	//	Vector2D size = AppWindow::getScreenSize();

	//	ImGui::SetNextWindowPos(ImVec2(30, 100), 0, ImVec2(0.5f, 0.5f));
	//	ImGui::OpenPopup("Spawn Cube");
	//	ImGui::BeginPopupModal("Spawn Cube");
	//	
	//	if (ImGui::Button("Okay", ImVec2(100, 30))) m_popup_toggle = false;
	//	ImGui::EndPopup();
	//}
}

void Scene11::shadowRenderPass(float delta)
{
}

void Scene11::mainRenderPass(float delta)
{
	Vector3D campos = CameraManager::get()->getCamera().getTranslation();
	//m_model->renderMesh(delta, Vector3D(1, 1, 1), Vector3D(0, 0, 2), Vector3D(0, 180 * 0.01745f, 0), Shaders::LAMBERT_RIMLIGHT);

	for (int i = 0; i < m_cubes.size(); i++)
	{
		m_cubes[i]->render(Shaders::LAMBERT_SPECULAR, false);
	}

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D->getShaderResourceView());
	m_sky->renderMesh(delta, Vector3D(700, 700, 700), campos, Vector3D(0, 0, 0), Shaders::WEATHER_ATMOSPHERE, false);
	m_floor->renderMesh(delta, Vector3D(10, 10, 10), Vector3D(campos.m_x, 0, campos.m_z), Vector3D(0, 0, 0), Shaders::FLAT, false);


}