#include "Scene09.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "ConstantBufferSystem.h"
#include "Sprite.h"
#include "Noise.h"
#include "Texture3D.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"

Scene09::Scene09(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(0, 0, -5));

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\cube.fbx", true, nullptr, D3D11_CULL_BACK);

	m_global_light_rotation = Vector2D(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vector3D(0.2, 0.2, 1.0);
	m_ambient_light_color = Vector3D(1.0, 1.0, 0.4);

	m_noise.m_noise_type = Vector4D(0, 0, 0, 1);
	m_noise.m_show_rgba = Vector4D(1, 0, 0, 0);

	m_noise.m_vor_amplitude = 1.0f;
	m_noise.m_vor_frequency = 4.0f;
	m_int_vor_frequency = 4;
	m_noise.m_vor_gain = 0.3f;
	m_noise.m_vor_lacunarity = 2.0f;
	m_noise.m_vor_octaves = 1;
	m_int_vor_octave = 1;
	m_noise.m_vor_cell_size = 0.25f;

	m_noise.m_per_amplitude = 1.0f;
	m_noise.m_per_frequency = 4.0f;
	m_int_per_frequency = 4;
	m_noise.m_per_gain = 0.3f;
	m_noise.m_per_lacunarity = 2.0f;
	m_noise.m_per_octaves = 1;
	m_int_per_octave = 1;
	m_noise.m_per_cell_size = 0.6f;


	m_cloud_props.m_cloud_density = 0.06f;
	m_cloud_props.m_cloud_position = Vector3D(0, 0, 0);
	m_cloud_props.m_cloud_size = Vector3D(150, 30, 150);
	m_cloud_props.m_vertical_fade = 0.4f;
	m_cloud_props.m_horizontal_fade = 0.8f;
	m_cloud_props.m_per_pixel_fade_threshhold = 0.15f;
	m_cloud_props.m_per_sample_fade_threshhold = 0.0f;
	m_cloud_props.m_sampling_resolution = Vector4D(30, 30, 30, 30);
	m_cloud_props.m_sampling_weight = Vector4D(0.4, 0.3, 0.2, 0.1);
	m_cloud_props.m_speed = 0;

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));

	m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));
}

Scene09::~Scene09()
{

}

void Scene09::update(float delta, const float& width, const float& height)
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

void Scene09::imGuiRender()
{
	//start the ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(400, 500));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	//create the test window
	ImGui::Begin("Test Window");
	ImGui::Text("Press 1 key to");
	ImGui::Text("display the mouse");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);

	if (ImGui::CollapsingHeader("Noise Settings"))
	{
		VectorToArray v(&m_cloud_props.m_cloud_position);

		v = VectorToArray(&m_cloud_props.m_move_dir);

		ImGui::DragFloat("Cell Size", &m_noise.m_per_cell_size, 0.05f, 0);
		ImGui::DragInt("Octaves", &m_int_per_octave, 0.05f, 0.00f, 10.0f);
		m_noise.m_per_octaves = m_int_per_octave;
		ImGui::DragFloat("Amplitude", &m_noise.m_per_amplitude, 0.005f, 0.00f, 5.0f);
		ImGui::DragFloat("Gain", &m_noise.m_per_gain, 0.005f, -10.00f, 10.0f);
	}

	if (ImGui::CollapsingHeader("Cloud Settings"))
	{
		VectorToArray v(&m_cloud_props.m_cloud_position);

		v = VectorToArray(&m_cloud_props.m_sampling_resolution);
		ImGui::DragFloat4("Sample Resolution", v.setArray(), 0.5f, 1.0f);

		v = VectorToArray(&m_cloud_props.m_sampling_weight);
		ImGui::DragFloat4("Sample Weight", v.setArray(), 0.02f, 0.0f, 1.0f);

		ImGui::DragFloat("Density", &m_cloud_props.m_cloud_density, 0.01f, 0, 1.0);
		ImGui::DragFloat("Per Pixel Fade Threshhold", &m_cloud_props.m_per_pixel_fade_threshhold, 0.01f, 0, 1.0);
		ImGui::DragFloat("Per Sample Fade Threshhold", &m_cloud_props.m_per_sample_fade_threshhold, 0.01f, 0, 1.0);

		v = VectorToArray(&m_cloud_props.m_move_dir);
		ImGui::DragFloat3("Movement Direction", v.setArray(), 0.02f, -1.0f, 1.0f);
		ImGui::DragFloat("Movement Speed", &m_cloud_props.m_speed, 0.05f, 0, 20.0);
	}

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

void Scene09::shadowRenderPass(float delta)
{
}

void Scene09::mainRenderPass(float delta)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D->getShaderResourceView());
	m_sky->renderMesh(delta, Vector3D(700, 700, 700), CameraManager::get()->getCamera().getTranslation(), Vector3D(0, 0, 0), Shaders::WEATHER_ATMOSPHERE);
	m_model->renderMesh(delta, Vector3D(10, 10, 1), Vector3D(0, 0, 0), Vector3D(0, 0, 0), Shaders::VOLUME_CLOUD);

}