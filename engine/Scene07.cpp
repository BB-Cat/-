#include "Scene07.h"
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

Scene07::Scene07(SceneManager* sm) : Scene(sm)
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
	m_noise.m_per_cell_size = 0.25f;

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));
}

Scene07::~Scene07()
{

}

void Scene07::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->setSpeed(m_speed);
	CameraManager::get()->update(delta, width, height);

	m_scene_light_dir = Vector3D(sinf(m_global_light_rotation.m_x), m_global_light_rotation.m_y, cosf(m_global_light_rotation.m_x));
	m_scene_light_dir.normalize();
	Lighting::get()->updateSceneLight(m_scene_light_dir, m_light_color, m_global_light_strength, m_ambient_light_color);

	if (m_tex == nullptr)
	{
		//we want the noise types to total 1.0 for the noise buffer update, 
		//so we will temporarily change the value here and then return it
		//so that imgui is not confused.
		Vector4D temp = m_noise.m_noise_type;
		float total_val = temp.m_x + temp.m_y + temp.m_z + temp.m_w;
		m_noise.m_noise_type = m_noise.m_noise_type / total_val;

		GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSNoiseBuffer(m_noise);
		m_noise.m_noise_type = temp;
		//-------------------------------------------------------//
	}
	else
	{
		GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetPSNoiseBuffer(m_noise);
	}

	m_timer++;
}

void Scene07::imGuiRender()
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
	//ImGui::DragInt("LOD", &m_toggle_HD, 0.005f, 0, 2);
	//ImGui::DragFloat("Camera Speed", &m_speed, 0.001f, 0.05f, 2.0f);

	//VectorToArray v(&m_global_light_rotation);
	//ImGui::DragFloat2("Light Direction", v.setArray(), 0.01f, -6.283f, 6.283f);

	//v = VectorToArray(&m_light_color);
	//ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	//ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	//v = VectorToArray(&m_ambient_light_color);
	//ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);

	if (m_tex == nullptr)
	{
		ImGui::Text("Noise settings");

		if (ImGui::CollapsingHeader("Voronoi Noise"))
		{
			ImGui::DragInt("Octaves", &m_int_vor_octave, 0.05f, 0.00f, 10.0f);
			m_noise.m_vor_octaves = m_int_vor_octave;
			ImGui::DragInt("Frequency", &m_int_vor_frequency, 0.05f, 0.00f, 10.0f);
			m_noise.m_vor_frequency = m_int_vor_frequency;
			ImGui::DragFloat("Amplitude", &m_noise.m_vor_amplitude, 0.005f, 0.00f, 5.0f);
			ImGui::DragFloat("Gain", &m_noise.m_vor_gain, 0.005f, -10.00f, 10.0f);
			
			ImGui::Text("Lacunarity : %.2f", m_noise.m_vor_lacunarity);
			if (ImGui::Button("Increase Lacunarity", ImVec2(150, 30))) m_noise.m_vor_lacunarity *= 2;
			ImGui::SameLine();
			if (ImGui::Button("Decrease Lacunarity", ImVec2(150, 30))) m_noise.m_vor_lacunarity /= 2;
		}

		if (ImGui::CollapsingHeader("Perlin Noise"))
		{
			ImGui::DragInt("Octaves", &m_int_per_octave, 0.05f, 0.00f, 10.0f);
			m_noise.m_per_octaves = m_int_per_octave;
			ImGui::DragInt("Frequency", &m_int_per_frequency, 0.05f, 0.00f, 10.0f);
			m_noise.m_per_frequency = m_int_per_frequency;
			ImGui::DragFloat("Amplitude", &m_noise.m_per_amplitude, 0.005f, 0.00f, 5.0f);
			ImGui::DragFloat("Gain", &m_noise.m_per_gain, 0.005f, -10.00f, 10.0f);

			ImGui::Text("Lacunarity : %.2f", m_noise.m_per_lacunarity);
			if (ImGui::Button("Increase Lacunarity", ImVec2(150, 30))) m_noise.m_per_lacunarity *= 2;
			ImGui::SameLine();
			if (ImGui::Button("Decrease Lacunarity", ImVec2(150, 30))) m_noise.m_per_lacunarity /= 2;
		}

		//m_noise.lacunarity = m_noise.lacunarity * (m_noise.lacunarity <= 1.0f) + 1.0f * (m_noise.lacunarity > 1.0f);

		if (ImGui::CollapsingHeader("Cell Size"))
		{
			ImGui::TextWrapped("!! Cell Size settings only effect the shader, not the output texture result.");
			if (ImGui::Button("Increase Voronoi Cell Size", ImVec2(150, 30))) m_noise.m_vor_cell_size *= 2;
			ImGui::SameLine();
			if (ImGui::Button("Decrease Voronoi Cell Size", ImVec2(150, 30))) m_noise.m_vor_cell_size /= 2;

			if (ImGui::Button("Increase Perlin Cell Size", ImVec2(150, 30))) m_noise.m_vor_cell_size *= 2;
			ImGui::SameLine();
			if (ImGui::Button("Decrease Perlin Cell Size", ImVec2(150, 30))) m_noise.m_vor_cell_size /= 2;
		}

		ImGui::InputFloat("Set Seed", &m_seed);

		VectorToArray v(&m_noise.m_noise_type);
		ImGui::SliderFloat4("Noise Types", v.setArray(), 0, 1.0f, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);

		if (ImGui::Button("Generate 3D Texture", ImVec2(200, 30))) load3DTexture();
		//ImGui::InputInt3("Texture Dimensions", 0);
	}
	else
	{
		ImGui::Text("3D Texture");

		if (ImGui::Button("Show R", ImVec2(70, 30))) m_noise.m_show_rgba = Vector4D(1, 0, 0, 0);
		ImGui::SameLine();
		if (ImGui::Button("Show G", ImVec2(70, 30))) m_noise.m_show_rgba = Vector4D(0, 1, 0, 0);
		ImGui::SameLine();
		if (ImGui::Button("Show B", ImVec2(70, 30))) m_noise.m_show_rgba = Vector4D(0, 0, 1, 0);
		ImGui::SameLine();
		if (ImGui::Button("Show A", ImVec2(70, 30))) m_noise.m_show_rgba = Vector4D(0, 0, 0, 1);


		if (ImGui::Button("Save File", ImVec2(200, 150))) m_tex->outputFile();
		if (ImGui::Button("Return", ImVec2(200, 30))) m_tex.reset();
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

void Scene07::shadowRenderPass(float delta)
{
}

void Scene07::mainRenderPass(float delta)
{
	static float move = 0.0f;

	if (AppWindow::getKeyState('I')) move += 0.01f;
	if (AppWindow::getKeyState('K')) move -= 0.01f;

	if (m_tex == nullptr)
	{
		m_sky->renderMesh(delta, Vector3D(700, 700, 700), CameraManager::get()->getCamera().getTranslation(), Vector3D(0, 0, 0), Shaders::ATMOSPHERE);
		m_model->renderMesh(delta, Vector3D(3, 3, 3), Vector3D(0, 0, move), Vector3D(0, 0, 0), Shaders::DYNAMIC_NOISE);
	}
	else
	{

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex->getShaderResourceView());
		m_model->renderMesh(delta, Vector3D(3, 3, 3), Vector3D(0, 0, move), Vector3D(0, 0, 0), Shaders::_3DTEX);
	}

}

void Scene07::load3DTexture()
{
	NoiseGenerator noise_gen;


	std::vector<float> noise;
	Vector3D size_3d(32.0f, 32.0f, 32.0f);

	//int division_count = m_noise.frequency * std::pow(m_noise.lacunarity, m_noise.octaves);
	//float freq = m_noise.vor_frequency * (m_noise.vor_frequency > m_noise.per_frequency) + 
	//	m_noise.per_frequency * (m_noise.per_frequency > m_noise.vor_frequency);
	int division_amount = size_3d.m_x / m_noise.m_vor_frequency;


	//int period = 8;
	int step = 0;

	float noise_balance_total = m_noise.m_noise_type.m_x + m_noise.m_noise_type.m_y + m_noise.m_noise_type.m_z + m_noise.m_noise_type.m_w;
	float xbal = m_noise.m_noise_type.m_x / noise_balance_total;
	float ybal = m_noise.m_noise_type.m_y / noise_balance_total;
	float zbal = m_noise.m_noise_type.m_z / noise_balance_total;
	float wbal = m_noise.m_noise_type.m_w / noise_balance_total;

	Vector3D pos;
	float val = 0;

	for (int i = 0; i < size_3d.m_z; i++)
	{
		for (int j = 0; j < size_3d.m_y; j++)
		{
		
			for (int k = 0; k < size_3d.m_x; k++)
			{

				pos = Vector3D(k, j, i);
				//R Output

				//change the seed for r g b and a
				noise_gen.setSeed(m_seed);
				if (wbal > 0)
				{
					val += (1.0f - noise_gen.brownianTiledVoronoi(pos / division_amount, m_noise.m_vor_octaves,
						m_noise.m_vor_frequency, m_noise.m_vor_gain, m_noise.m_vor_lacunarity, m_noise.m_vor_amplitude).m_x) * wbal;
				}

				if (zbal > 0)
				{
					val += (noise_gen.brownianTiledPerlin(pos / division_amount, m_noise.m_per_octaves,
						m_noise.m_per_frequency, m_noise.m_per_gain, m_noise.m_per_lacunarity, m_noise.m_per_amplitude).m_x + 0.5) * zbal;
				}

				noise.push_back(val);

				//G Output
				noise_gen.setSeed(m_seed * 2);
				if (wbal > 0)
				{
					val = (1.0f - noise_gen.brownianTiledVoronoi(pos / division_amount, m_noise.m_vor_octaves,
						m_noise.m_vor_frequency, m_noise.m_vor_gain, m_noise.m_vor_lacunarity, m_noise.m_vor_amplitude).m_x) * wbal;
				}

				noise.push_back(val);

				//B Output
				noise_gen.setSeed(m_seed * 3);
				if (wbal > 0)
				{
					val = (1.0f - noise_gen.brownianTiledVoronoi(pos * 2 / division_amount, m_noise.m_vor_octaves,
						m_noise.m_vor_frequency, m_noise.m_vor_gain, m_noise.m_vor_lacunarity, m_noise.m_vor_amplitude).m_x) * wbal;
				}

				noise.push_back(val);
				
				//A Output
				noise_gen.setSeed(m_seed * 4);
				if (wbal > 0)
				{
					val = (1.0f - noise_gen.brownianTiledVoronoi(pos * 3 / division_amount, m_noise.m_vor_octaves,
						m_noise.m_vor_frequency, m_noise.m_vor_gain, m_noise.m_vor_lacunarity, m_noise.m_vor_amplitude).m_x) * wbal;
				}

				noise.push_back(val);
				val = 0;
				//noise.push_back(val);
				//noise.push_back(val);
				//noise.push_back(val);
			}
		}
	}

	m_tex = std::shared_ptr<Texture3D>(new Texture3D(size_3d, noise));
}
