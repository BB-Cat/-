#include "Scene17.h"
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


Scene17::Scene17(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(FREE);


	m_light = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_BACK);
	m_light->setColor(Vec3(1, 1, 1));
	
	m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\cube.fbx", true, nullptr, D3D11_CULL_BACK);

	m_global_light_rotation = Vec2(1.7f, -0.33f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(1.0, 0.85, 0.8);


	m_raymarch_buffer.m_params.m_y = 6;
	generateCloud();

	//position
	m_cloud_props.m_cloud_position = Vec3(0);

	//ray offset
	m_cloud_props.m_ray_offset_strength = 9.0f;
	//light steps - 2
	m_cloud_props.m_light_stepcount = 2;

	m_default = m_cloud_props;

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



	m_tex3D_main = std::shared_ptr<Texture3D>(new Texture3D("voronoiPerlin128x.txt"));
	//m_tex3D_main = std::shared_ptr<Texture3D>(new Texture3D("Voronoi32x.txt"));
	m_tex3D_detail = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));

	m_blue_noise = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Noise\\bluenoise1.png");

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.1, 0.1, 0.4));

	CameraManager::get()->setCamPos(Vec3(907.7f, 532, 487));
	CameraManager::get()->setCamRot(Vec2(6.7f, 42.0f));
}

Scene17::~Scene17()
{

}

void Scene17::update(float delta)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta);

	m_scene_light_dir = Vec3(sinf(m_timer) * 3, abs(sinf(m_timer * 0.8f)), cosf(m_timer) * 3);
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCSRaymarchBuffer(m_raymarch_buffer);

	m_timer += delta;
}

void Scene17::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------

	ImGui::SetNextWindowSize(ImVec2(215, 45));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, 65));
	ImGui::SetNextWindowBgAlpha(0.6f);

	ImGui::Begin("CloudSpheres", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::SliderInt("Spheres", &m_spherecount, 0, MAX_SPHERES);
	m_raymarch_buffer.m_params.m_y = m_spherecount;

	if (ImGui::Button("Display Distance", ImVec2(150, 20))) m_raymarch_buffer.m_params.m_w = 3;
	if (ImGui::Button("Display Normals", ImVec2(150, 20))) m_raymarch_buffer.m_params.m_w = 2;
	if (ImGui::Button("Display Distance in Cloud", ImVec2(150, 20))) m_raymarch_buffer.m_params.m_w = 1;
	if (ImGui::Button("Display Noise", ImVec2(150, 20))) m_raymarch_buffer.m_params.m_w = 0;

	ImGui::NewLine();

	if (ImGui::Button("Rearrange Spheres", ImVec2(200, 20))) generateCloud();



	ImGui::End();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Cloud Popup");
		ImGui::BeginPopupModal("Cloud Popup");

		ImGui::TextWrapped("This scene is still in progress. I am going to try using raymarched spheres to build cloud shapes (Inspired by CyGames method!)");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

}

void Scene17::shadowRenderPass(float delta)
{
}

void Scene17::mainRenderPass(float delta)
{
	//GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D_main->getShaderResourceView());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseNormalGlossTexPS
	(m_tex3D_main->getShaderResourceView(), m_tex3D_detail->getShaderResourceView(), m_blue_noise->getSRV());

	if (m_show_vertices)
	{
		m_model->renderMesh(delta, m_cloud_props.m_cloud_size.xyz(), m_cloud_props.m_cloud_position.xyz(), Vec3(0, 0, 0), Shaders::LAMBERT_RIMLIGHT);
	}
	else
	{
		m_model->renderMesh(delta, m_cloud_props.m_cloud_size.xyz(), m_cloud_props.m_cloud_position.xyz(), Vec3(0, 0, 0), Shaders::RT_SPHERE_CLOUDS);
	}

}

void Scene17::generateCloud()
{
	//generate randum radii
	float radii[MAX_SPHERES];
	for (int i = 0; i < MAX_SPHERES; i++) radii[i] = (rand() % 50 / 100.0f + 0.5f) * 100;

	Vec2 pos[MAX_SPHERES];
	Vec2 newpos;
	float rangebias = 0.2f;
	//generate positions that dont collide with eachother.
	for (int i = 0; i < MAX_SPHERES; i++)
	{
		newpos = Vec2((rand() % 100 / 100.0f - 0.5f) * 100 * rangebias, (rand() % 100 / 100.0f - 0.5f) * rangebias * 100);

		//make sure it's not colliding with other spheres too much, otherwise we loop and increase the range bias slightly
		bool check = false;
		for (int j = 0; j < i; j++) if ((pos[j] - newpos).length() < radii[j]) check = true;
		if (check)
		{
			i--;
			rangebias += 0.1f;
		}
		else pos[i] = newpos;
	}

	for (int i = 0; i < MAX_SPHERES; i++)
	{
		m_raymarch_buffer.m_sphere_pos_and_radius[i].m_x = pos[i].x;
		m_raymarch_buffer.m_sphere_pos_and_radius[i].m_z = pos[i].y;

		m_raymarch_buffer.m_sphere_pos_and_radius[i].m_w = radii[i]; //radius from 0.5 to 1.0
		m_raymarch_buffer.m_sphere_pos_and_radius[i].m_y = radii[i] * sin(rand() % 100 / 100.0f * 6.283f); //height from radius

	}

	Vec3 size = Vec3(0);
	Vec3 min = Vec3(0);
	Vec3 max = Vec3(0);
	for (int i = 0; i < MAX_SPHERES; i++)
	{
		Vector4D* vec = &m_raymarch_buffer.m_sphere_pos_and_radius[i];
		float r = vec->m_w;

		float x = vec->m_x;
		if (x + r > max.x) max.x = x + r;
		if (x - r < min.x) min.x = x - r;
		float y = vec->m_y;
		if (y + r > max.y) max.y = y + r;
		if (y - r < min.y) min.y = y - r;
		float z = vec->m_z;
		if (z + r > max.z) max.z = z + r;
		if (z - r < min.z) min.z = z - r;
	}

	size = max - min;
	//size
	m_cloud_props.m_cloud_size = size;
}
