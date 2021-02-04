#include "Scene08.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "MyAudio.h"
#include "ConstantBufferSystem.h"
#include "Sprite.h"
#include "DeviceContext.h"
#include "Noise.h"
#include "Texture.h"


Scene08::Scene08(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(FREE);


	m_light = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_BACK);
	m_light->setColor(Vec3(1, 1, 1));
	
	m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\cube.fbx", true, nullptr, D3D11_CULL_BACK);

	m_global_light_rotation = Vec2(1.7f, -0.33f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(1.0, 0.85, 0.8);

	//initial cloud property settings
	m_cloud_props.m_cloud_density = 2.0f;
	m_cloud_props.m_sampling_resolution = Vector4D(1.0f, 0, 0, 0);
	m_cloud_props.m_sampling_weight = Vector4D(1.0f, 0.0f, 0.19f, 0.42f);
	m_cloud_props.m_speed = 0.19f;
	m_cloud_props.m_move_dir = Vec3(-0.34f, -0.071f, -0.2f);
	m_cloud_props.m_ray_offset_strength = 9.95f;
	m_cloud_props.m_phase_parameters = Vector4D(0.00f, 0.493f, 0.043f, 0.058f);
	m_cloud_props.m_density_offset = 60.0f;
	m_cloud_props.m_detail_noise_scale = 3.0f;
	m_cloud_props.m_detail_speed = 3.9f;
	m_cloud_props.m_detail_sampling_weight = Vector4D(1.0f, 0.0f, 1.0f, 1.0f);
	m_cloud_props.m_detail_noise_weight = 20.0f;
	m_cloud_props.m_darkness_threshold = 1.35f;
	m_cloud_props.m_light_absorption_towards_sun = 0.01f;
	m_cloud_props.m_light_absorption_through_cloud = 0.1f;

	//completely static
	//position
	m_cloud_props.m_cloud_position = Vec3(0, -120.0f, 0);
	//size
	m_cloud_props.m_cloud_size = Vec3(1000, 600, 1000);
	//ray offset
	m_cloud_props.m_ray_offset_strength = 9.0f;
	//light steps - 2
	m_cloud_props.m_light_stepcount = 2;

	m_default = m_cloud_props;

	//preset 1
	m_preset1 = m_cloud_props;
	//texture resolution
	m_preset1.m_sampling_resolution.m_x = 1.191f;
	// sample weight
	m_preset1.m_sampling_weight = Vector4D(1, 0.5f, 0, 0.33f);
	//texture density
	m_preset1.m_cloud_density = 0.335f;
	//movement direction
	m_preset1.m_move_dir = Vec3(0.088f, -0.2f, -0.2f);
	//movement speed
	m_preset1.m_speed = 0.39f;
	//phase parameters
	m_preset1.m_phase_parameters = Vector4D(0, 0, 0.042f, 0);
	//density offset
	m_preset1.m_density_offset = 41.7f;
	//detail scale
	m_preset1.m_detail_noise_scale = 3.61f;
	//detail speed
	m_preset1.m_detail_speed = 3.94f;
	//detail sampling
	m_preset1.m_detail_sampling_weight = Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
	//detail weight 
	m_preset1.m_detail_noise_weight = 20.0f;
	//darkness threshhold
	m_preset1.m_darkness_threshold = 2.75f;
	//light absorption 1
	m_preset1.m_light_absorption_through_cloud = 0.11f;
	//light absorption 2
	m_preset1.m_light_absorption_towards_sun = 0.89f;


	//preset 2
	m_preset2 = m_cloud_props;
	//texture resolution
	m_preset2.m_sampling_resolution.m_x = 1.13f;
	//movement direction
	m_preset2.m_move_dir = Vec3(-0.112f, 0, 0.127f);
	//movement speed
	m_preset2.m_speed = 1.63f;
	// sample weight
	m_preset2.m_sampling_weight = Vector4D(1, 0.5f, 0, 0.33f);
	//texture density
	m_preset2.m_cloud_density = 0.5f;
	//phase parameters
	m_preset2.m_phase_parameters = Vector4D(0.688f, 1.0f, 0.042f, 0.18f);
	//density offset
	m_preset2.m_density_offset = 35.854f;
	//detail scale
	m_preset2.m_detail_noise_scale = 2.74f;
	//detail speed
	m_preset2.m_detail_speed = 2.927f;
	//detail sampling
	m_preset2.m_detail_sampling_weight = Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
	//detail weight 
	m_preset2.m_detail_noise_weight = 28.78f;
	//darkness threshhold
	m_preset2.m_darkness_threshold = 1.2f;
	//light absorption 1
	m_preset2.m_light_absorption_through_cloud = 0.09f;
	//light absorption 2
	m_preset2.m_light_absorption_towards_sun = 0.0f;



	//preset 3
	m_preset3 = m_cloud_props;
	//texture resolution
	m_preset3.m_sampling_resolution.m_x = 1.6f;
	//movement direction
	m_preset3.m_move_dir = Vec3(0.01f, -0.2f, 0.01f);
	//movement speed
	m_preset3.m_speed = 0.87f;
	// sample weight
	m_preset3.m_sampling_weight = Vector4D(1, 1, 0, 1);
	//texture density
	m_preset3.m_cloud_density = 2.0f;
	//phase parameters
	m_preset3.m_phase_parameters = Vector4D(0.375f, 0.75f, 0.083f, 0.063f);
	//density offset
	m_preset3.m_density_offset = 27.317;
	//detail scale
	m_preset3.m_detail_noise_scale = 1.114f;
	//detail speed
	m_preset3.m_detail_speed = 8.232f;
	//detail sampling
	m_preset3.m_detail_sampling_weight = Vector4D(1.0f, 0.0f, 0.0f, 1.0f);
	//detail weight 
	m_preset3.m_detail_noise_weight = 22.195f;
	//darkness threshhold
	m_preset3.m_darkness_threshold = 1.6f;
	//light absorption 1
	m_preset3.m_light_absorption_through_cloud = 0.19f;
	//light absorption 2
	m_preset3.m_light_absorption_towards_sun = 0.03f;


	//preset 4
	m_preset4 = m_cloud_props;
	// sample weight
	m_preset4.m_sampling_weight = Vector4D(1.0f, 1.0f, 0, 0.0f);
	//texture density
	m_preset4.m_cloud_density = 3.585f;
	//phase parameters
	m_preset4.m_phase_parameters = Vector4D(0.521, 0.75f, 0, 0.104f);
	//density offset
	m_preset4.m_density_offset = 22.927f;
	//detail scale
	m_preset4.m_detail_noise_scale = 2.658f;
	//detail speed
	m_preset4.m_detail_speed = 12.378f;
	//detail sampling
	m_preset4.m_detail_sampling_weight = Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
	//detail weight 
	m_preset4.m_detail_noise_weight = 20.0f;
	//darkness threshhold
	m_preset4.m_darkness_threshold = 1.75f;
	//light absorption 1
	m_preset4.m_light_absorption_through_cloud = 0.18f;
	//light absorption 2
	m_preset4.m_light_absorption_towards_sun = 0.02f;

	//m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("voronoiPerlin128x.txt"));
	m_tex3D_main = std::shared_ptr<Texture3D>(new Texture3D("Voronoi32x.txt"));
	m_tex3D_detail = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));

	m_blue_noise = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Noise\\bluenoise1.png");

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.1, 0.1, 0.4));

	CameraManager::get()->setCamPos(Vec3(907.7f, 532, 487));
	CameraManager::get()->setCamRot(Vec2(6.7f, 42.0f));
}

Scene08::~Scene08()
{

}

void Scene08::update(float delta)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta);

	//m_scene_light_dir = Vec3(sinf(m_global_light_rotation.x), m_global_light_rotation.y, cosf(m_global_light_rotation.x));

	m_scene_light_dir = Vec3(sinf(m_timer) * 3, abs(sinf(m_timer * 0.8f)), cosf(m_timer) * 3);
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);


	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);

	m_timer += delta;
}

void Scene08::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------

	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	ImGui::End();




	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);
	//ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);


	//ImGui::SetNextWindowSize(ImVec2(530, 200));
	ImGui::SetNextWindowPos(ImVec2(260, 20));
	ImGui::SetNextWindowBgAlpha(0.2f);

	ImGui::Begin("Presets", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

	ImVec2 size = ImVec2(80, 30);
	if (ImGui::Button("Default", size))
	{
		float temp = m_cloud_props.m_time;
		m_cloud_props = m_default;
		m_cloud_props.m_time = temp;
	}

	ImGui::SameLine();

	if (ImGui::Button("Preset1", size))
	{
		float temp = m_cloud_props.m_time;
		m_cloud_props = m_preset1;
		m_cloud_props.m_time = temp;
	}

	ImGui::SameLine();

	if (ImGui::Button("Preset2", size))
	{
		float temp = m_cloud_props.m_time;
		m_cloud_props = m_preset2;
		m_cloud_props.m_time = temp;
	}

	ImGui::SameLine();

	if (ImGui::Button("Preset3", size))
	{
		float temp = m_cloud_props.m_time;
		m_cloud_props = m_preset3;
		m_cloud_props.m_time = temp;
	}

	ImGui::SameLine();

	if (ImGui::Button("Preset4", size))
	{
		float temp = m_cloud_props.m_time;
		m_cloud_props = m_preset4;
		m_cloud_props.m_time = temp;
	}

	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(530, 200));
	ImGui::SetNextWindowPos(ImVec2(460, 520));
	ImGui::SetNextWindowBgAlpha(0.6f);

	//create the test window
	ImGui::Begin("Clouds", 0, ImGuiWindowFlags_NoDecoration);

	//if (ImGui::CollapsingHeader("Cloud Settings"))
	//{
		VectorToArray v(&m_cloud_props.m_cloud_position);

		v = VectorToArray(&m_cloud_props.m_sampling_resolution);
		//ImGui::DragFloat("Sample Resolution", v.setArray(), 0.1f, 0.0f, 200.0f);
		ImGui::SliderFloat("Texture Resolution", v.setArray(), 0.5f, 3.0f);

		v = VectorToArray(&m_cloud_props.m_sampling_weight);
		ImGui::DragFloat4("Sample Weight", v.setArray(), 0.01f, 0.0f, 1.0f);
		
		//ImGui::DragFloat("Density", &m_cloud_props.m_cloud_density, 0.01f, 0);
		ImGui::SliderFloat("Texture Density", &m_cloud_props.m_cloud_density, 0.01f, 20.0f);

		v = VectorToArray(&m_cloud_props.m_move_dir);
		ImGui::DragFloat3("Movement Direction", v.setArray(), 0.001f, -0.2f, 0.2f);
		ImGui::DragFloat("Movement Speed", &m_cloud_props.m_speed, 0.01f, 0, 20.0);

		//ImGui::DragFloat("Ray Offset", &m_cloud_props.m_ray_offset_strength, 0.01f, 1.0f);
		ImGui::SliderFloat("Ray Offset", &m_cloud_props.m_ray_offset_strength, 0.5f, 30.0f);

		v = VectorToArray(&m_cloud_props.m_phase_parameters);
		//ImGui::DragFloat4("Phase Parameters", v.setArray(), 0.01f, 0.0f, 1.0f);
		//ImGui::SliderFloat4("Phase Parameters", v.setArray(), 0.0f, 1.0f);

		//ImGui::DragFloat("Density Offset", &m_cloud_props.m_density_offset, 0.01f, 0.0f);
		ImGui::SliderFloat("Density Offset", &m_cloud_props.m_density_offset, 0.0f, 60.0f);

		//ImGui::DragFloat("Detail Noise Scale", &m_cloud_props.m_detail_noise_scale, 0.01f, 0.0f);
		ImGui::SliderFloat("Detail Noise Scale", &m_cloud_props.m_detail_noise_scale, 0.1f, 3.0f);


		//ImGui::DragFloat("Detail Speed", &m_cloud_props.m_detail_speed, 0.01f, 0.0f);
		ImGui::SliderFloat("Detail Noise Speed", &m_cloud_props.m_detail_speed, 0.0f, 15.0f);

		v = VectorToArray(&m_cloud_props.m_detail_sampling_weight);
		//ImGui::DragFloat4("Detail Sampling Weight", v.setArray(), 0.01f, 0.0f, 1.0f);


		//ImGui::DragFloat("Detail Noise Weight", &m_cloud_props.m_detail_noise_weight, 0.02f, 0.0f);
		ImGui::SliderFloat("Detail Weight", &m_cloud_props.m_detail_noise_weight, 20.0f, 200.0f);


		//int light = m_cloud_props.m_light_stepcount;
		//ImGui::DragInt("Light Step Count", &light, 0.05f, 0.0f);
		//ImGui::SliderInt("Light Steps", &light, 0.0f, 4.0f);
		//m_cloud_props.m_light_stepcount = light;
		//m_cloud_props.m_darkness_threshold = 0.3f;

		//ImGui::DragFloat("Darkness Threshold", &m_cloud_props.m_darkness_threshold, 0.05f, 0.0f);
		//m_cloud_props.m_light_absorption_towards_sun = 0.1f;
		//ImGui::DragFloat("Light Absorption Sowards Source Light", &m_cloud_props.m_light_absorption_towards_sun, 0.01f, 0.0f);
		//m_cloud_props.m_light_absorption_through_cloud = 0.1f;
		//ImGui::DragFloat("Light Absorption Through Cloud", &m_cloud_props.m_light_absorption_through_cloud, 0.01f, 0.0f);


	//}

	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(460, 200));
	ImGui::SetNextWindowPos(ImVec2(0, 520));
	ImGui::SetNextWindowBgAlpha(0.6f);

	//create the test window
	ImGui::Begin("Lighting", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);

	//if(ImGui::CollapsingHeader("Lighting"))
	//{
		v = VectorToArray(&m_global_light_rotation);
		ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f);

		v = VectorToArray(&m_light_color);
		ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	//}

	//ImGui::Text("Time: %.3f", m_cloud_props.m_time);
	if(ImGui::Button("Show Vertexes", ImVec2(200, 30))) m_show_vertices = (!m_show_vertices);


	ImGui::End();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Cloud Popup");
		ImGui::BeginPopupModal("Cloud Popup");

		ImGui::TextWrapped("This scene uses 3D textures to create a volumetric cloud. There are buttons to see presets at the top of the screen.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

}

void Scene08::shadowRenderPass(float delta)
{
}

void Scene08::mainRenderPass(float delta)
{

	Vec3 lightpos = m_cloud_props.m_cloud_position.xyz() + (m_scene_light_dir * -1) * 1000;
	//Vector3D lightpos = m_cloud_props.m_cloud_position.xyz();
	int a = 3;
	m_light->renderMesh(delta, Vec3(100, 100, 100), lightpos, Vec3(0, 0, 0), Shaders::FLAT);

	//GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D_main->getShaderResourceView());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseNormalGlossTexPS
	(m_tex3D_main->getShaderResourceView(), m_tex3D_detail->getShaderResourceView(), m_blue_noise->getSRV());

	if (m_show_vertices)
	{
		m_model->renderMesh(delta, m_cloud_props.m_cloud_size.xyz(), m_cloud_props.m_cloud_position.xyz(), Vec3(0, 0, 0), Shaders::LAMBERT_RIMLIGHT);
	}
	else
	{
		m_model->renderMesh(delta, m_cloud_props.m_cloud_size.xyz(), m_cloud_props.m_cloud_position.xyz(), Vec3(0, 0, 0), Shaders::VOLUME_CLOUD);
	}

}