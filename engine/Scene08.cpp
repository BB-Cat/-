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


Scene08::Scene08(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(0, 3, -15));
	CameraManager::get()->setCamRot(Vector2D(0,0));

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\cube.fbx", true, nullptr, D3D11_CULL_BACK);

	m_global_light_rotation = Vector2D(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vector3D(0.2, 0.2, 1.0);
	m_ambient_light_color = Vector3D(1.0, 1.0, 0.4);


	//initial cloud property settings
	m_cloud_props.m_cloud_density = 0.02f;
	m_cloud_props.m_cloud_position = Vector3D(0, 0, 0);
	m_cloud_props.m_cloud_size = Vector3D(15, 10, 20);
	m_cloud_props.m_vertical_fade = 0.9f;
	m_cloud_props.m_horizontal_fade = 0.35f;
	m_cloud_props.m_per_pixel_fade_threshhold = 0.0f;
	m_cloud_props.m_per_sample_fade_threshhold = 0.5f;
	m_cloud_props.m_sampling_resolution = Vector4D(22, 9, 7, 10);
	m_cloud_props.m_sampling_weight = Vector4D(0.4, 0.3, 0.2, 0.1);
	m_cloud_props.m_speed = 2.6;
	m_cloud_props.m_move_dir = Vector3D(1, 0, 1);


	//m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("voronoiPerlin128x.txt"));
	m_tex3D = std::shared_ptr<Texture3D>(new Texture3D("Perlin32x.txt"));

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));
}

Scene08::~Scene08()
{

}

void Scene08::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, width, height);

	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);


	m_cloud_props.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSCloudBuffer(m_cloud_props);

	m_timer++;
}

void Scene08::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(400, 500));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Test Window");
	ImGui::Text("Press 1 key to");
	ImGui::Text("display the mouse");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);
	ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);

	if (ImGui::CollapsingHeader("Cloud Settings"))
	{
		VectorToArray v(&m_cloud_props.m_cloud_position);
		ImGui::DragFloat3("Position", v.setArray(), 0.5f, -300.0f, 300.0f);
		
		v = VectorToArray(&m_cloud_props.m_cloud_size);
		ImGui::DragFloat3("Size", v.setArray(), 1.5f, 1.0f, 500.0f);

		v = VectorToArray(&m_cloud_props.m_sampling_resolution);
		ImGui::DragFloat4("Sample Resolution", v.setArray(), 0.5f, 1.0f, 200.0f);

		v = VectorToArray(&m_cloud_props.m_sampling_weight);
		ImGui::DragFloat4("Sample Weight", v.setArray(), 0.02f, 0.0f, 1.0f);
		
		ImGui::DragFloat("Density", &m_cloud_props.m_cloud_density, 0.01f, 0, 1.0);
		ImGui::DragFloat("Vertical Fade", &m_cloud_props.m_vertical_fade, 0.01f, 0, 1.0);
		ImGui::DragFloat("Horizontal Fade", &m_cloud_props.m_horizontal_fade, 0.01f, 0, 1.0);
		ImGui::DragFloat("Per Pixel Fade Threshhold", &m_cloud_props.m_per_pixel_fade_threshhold, 0.01f, 0, 1.0);
		ImGui::DragFloat("Per Sample Fade Threshhold", &m_cloud_props.m_per_sample_fade_threshhold, 0.01f, 0, 1.0);
		ImGui::DragFloat("In Scattering Strength", &m_cloud_props.m_in_scattering_strength, 0.01f, 0, 1.0);
		ImGui::DragFloat("Out Scattering Strength", &m_cloud_props.m_out_scattering_strength, 0.01f, 0, 1.0);

		v = VectorToArray(&m_cloud_props.m_move_dir);
		ImGui::DragFloat3("Movement Direction", v.setArray(), 0.02f, -1.0f, 1.0f);
		ImGui::DragFloat("Movement Speed", &m_cloud_props.m_speed, 0.05f, 0, 20.0);
	}

	if(ImGui::CollapsingHeader("Lighting"))
	{
		VectorToArray v(&m_global_light_rotation);
		ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

		v = VectorToArray(&m_light_color);
		ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
		ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

		v = VectorToArray(&m_ambient_light_color);
		ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);
	}
	ImGui::Text("Time: %.3f", m_cloud_props.m_time);
	if(ImGui::Button("Show Vertexes", ImVec2(200, 30))) m_show_tex3D = (!m_show_tex3D);


	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Cloud Popup");
		ImGui::BeginPopupModal("Cloud Popup");

		ImGui::TextWrapped("This scene uses 3D textures to create a volumetric cloud.  This is an early test so it is slow.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();
}

void Scene08::shadowRenderPass(float delta)
{
}

void Scene08::mainRenderPass(float delta)
{

	//m_sky->renderMesh(delta, Vector3D(700, 700, 700), CameraManager::get()->getCamera().getTranslation(), Vector3D(0, 0, 0), Shaders::ATMOSPHERE);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex3D->getShaderResourceView());
	if (m_show_tex3D)
	{
		m_model->renderMesh(delta, m_cloud_props.m_cloud_size.xyz(), m_cloud_props.m_cloud_position.xyz(), Vector3D(0, 0, 0), Shaders::_3DTEX);
	}
	else
	{
		m_model->renderMesh(delta, m_cloud_props.m_cloud_size.xyz(), m_cloud_props.m_cloud_position.xyz(), Vector3D(0, 0, 0), Shaders::VOLUME_CLOUD);
	}

}