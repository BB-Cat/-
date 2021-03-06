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
	CameraManager::get()->setCamPos(Vec3(2.0, 3.8f, -4.23f));
	CameraManager::get()->setSpeed(0.2f);
	CameraManager::get()->setCamRot(Vec2(0.688f, -0.76f));

	m_sky = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\SkySphere\\sphere.fbx", true, nullptr, D3D11_CULL_FRONT);
	m_model = GraphicsEngine::get()->getSkinnedMeshManager()->createSkinnedMeshFromFile(L"..\\Assets\\cube.fbx", true, nullptr, D3D11_CULL_BACK);

	m_global_light_rotation = Vec2(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(0.2, 0.2, 1.0);
	m_ambient_light_color = Vec3(1.0, 1.0, 0.4);

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

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.1, 0.1, 0.4));
}

Scene07::~Scene07()
{

}

void Scene07::update(float delta)
{
	CameraManager::get()->update(delta);

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
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(400, 500));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Noise Window");

	if (ImGui::Button("Scene Select", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	if (ImGui::Button("Show Explanation", ImVec2(200, 30))) m_first_time = true;
	//if (ImGui::Button("Move Forward", ImVec2(100, 20)))
	//{
	//	m_is_move = 1;
	//}
	//if (ImGui::Button("Move Backward", ImVec2(100, 20)))
	//{
	//	m_is_move = -1;
	//}

	ImGui::NewLine();


	if (ImGui::Button("Move Noise", ImVec2(200, 20)))
	{
		m_is_move = 1;
	}

	if (ImGui::Button("Stop", ImVec2(200, 20)))
	{
		m_is_move = 0;
	}

	if (m_is_move)
	{
		if (m_move < -0.5f || m_move > 0.5f) m_is_move *= -1;


		m_move += 0.01f * m_is_move;
	}

	if (m_tex == nullptr)
	{
		ImGui::Text("Noise settings");

		ImGui::PushID("Vor");
		if (ImGui::CollapsingHeader("Voronoi Noise"))
		{
			ImGui::DragInt("Octaves", &m_int_vor_octave, 0.05f, 0.00f, 5.0f);
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
		ImGui::PopID();
		ImGui::PushID("Per");

		if (ImGui::CollapsingHeader("Perlin Noise"))
		{
			ImGui::DragInt("Octaves", &m_int_per_octave, 0.05f, 0.00f, 5.0f);
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
		ImGui::PopID();
		//m_noise.lacunarity = m_noise.lacunarity * (m_noise.lacunarity <= 1.0f) + 1.0f * (m_noise.lacunarity > 1.0f);

		if (ImGui::CollapsingHeader("Cell Size"))
		{
			ImGui::TextWrapped("!! Cell Size settings only effect the shader, not the output texture result.");
			if (ImGui::Button("Increase Cell Size", ImVec2(150, 30))) m_noise.m_vor_cell_size *= 2;
			ImGui::SameLine();
			if (ImGui::Button("Decrease Cell Size", ImVec2(150, 30))) m_noise.m_vor_cell_size /= 2;

		}

		ImGui::InputFloat("Set Seed", &m_seed);

		VectorToArray v(&m_noise.m_noise_type);
		//ImGui::SliderFloat4("Noise Types", v.setArray(), 0, 1.0f, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);

		ImGui::SliderFloat("Voronoi Amount", &m_noise.m_noise_type.m_w, 0.0f, 1.0f, nullptr, 1.0f);
		ImGui::SliderFloat("Perlin Amount", &m_noise.m_noise_type.m_z, 0.0f, 1.0f, nullptr, 1.0f);

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

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Noise Popup");
		ImGui::BeginPopupModal("Noise Popup");

		ImGui::TextWrapped("This scene generates noise which is used for volume rendering. You can save it to a 3D texture file. I made this scene before I understood compute shaders, so the texture creation is very slow.  I want to make a compute shader version to make it faster.");

		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();
}

void Scene07::shadowRenderPass(float delta)
{
}

void Scene07::mainRenderPass(float delta)
{

	if (m_tex == nullptr)
	{
		m_sky->renderMesh(delta, Vec3(700, 700, 700), CameraManager::get()->getCamera().getTranslation(), Vec3(0, 0, 0), Shaders::ATMOSPHERE);
		m_model->renderMesh(delta, Vec3(3, 3, 3), Vec3(0, 0, m_move), Vec3(0, 0, 0), Shaders::DYNAMIC_NOISE);
	}
	else
	{

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDiffuseTexPS(m_tex->getShaderResourceView());
		m_model->renderMesh(delta, Vec3(3, 3, 3), Vec3(0, 0, m_move), Vec3(0, 0, 0), Shaders::_3DTEX);
	}

}

void Scene07::load3DTexture()
{
	NoiseGenerator noise_gen;


	std::vector<float> noise;
	Vec3 size_3d(32.0f, 32.0f, 32.0f);

	//int division_count = m_noise.frequency * std::pow(m_noise.lacunarity, m_noise.octaves);
	//float freq = m_noise.vor_frequency * (m_noise.vor_frequency > m_noise.per_frequency) + 
	//	m_noise.per_frequency * (m_noise.per_frequency > m_noise.vor_frequency);
	int division_amount = size_3d.x / m_noise.m_vor_frequency;


	//int period = 8;
	int step = 0;

	float noise_balance_total = m_noise.m_noise_type.m_x + m_noise.m_noise_type.m_y + m_noise.m_noise_type.m_z + m_noise.m_noise_type.m_w;
	float xbal = m_noise.m_noise_type.m_x / noise_balance_total;
	float ybal = m_noise.m_noise_type.m_y / noise_balance_total;
	float zbal = m_noise.m_noise_type.m_z / noise_balance_total;
	float wbal = m_noise.m_noise_type.m_w / noise_balance_total;

	Vec3 pos;
	float val = 0;

	for (int i = 0; i < size_3d.z; i++)
	{
		for (int j = 0; j < size_3d.y; j++)
		{
		
			for (int k = 0; k < size_3d.x; k++)
			{

				pos = Vec3(k, j, i);
				//R Output

				//change the seed for r g b and a
				noise_gen.setSeed(m_seed);
				if (wbal > 0)
				{
					val += (1.0f - noise_gen.brownianTiledVoronoi(pos / division_amount, m_noise.m_vor_octaves,
						m_noise.m_vor_frequency, m_noise.m_vor_gain, m_noise.m_vor_lacunarity, m_noise.m_vor_amplitude).x) * wbal;
				}

				if (zbal > 0)
				{
					val += (noise_gen.brownianTiledPerlin(pos / division_amount, m_noise.m_per_octaves,
						m_noise.m_per_frequency, m_noise.m_per_gain, m_noise.m_per_lacunarity, m_noise.m_per_amplitude).x + 0.5) * zbal;
				}

				noise.push_back(val);

				//G Output
				noise_gen.setSeed(m_seed * 2);
				if (wbal > 0)
				{
					val = (1.0f - noise_gen.brownianTiledVoronoi(pos / division_amount, m_noise.m_vor_octaves,
						m_noise.m_vor_frequency, m_noise.m_vor_gain, m_noise.m_vor_lacunarity, m_noise.m_vor_amplitude).x) * wbal;
				}

				noise.push_back(val);

				//B Output
				noise_gen.setSeed(m_seed * 3);
				if (wbal > 0)
				{
					val = (1.0f - noise_gen.brownianTiledVoronoi(pos * 2 / division_amount, m_noise.m_vor_octaves,
						m_noise.m_vor_frequency, m_noise.m_vor_gain, m_noise.m_vor_lacunarity, m_noise.m_vor_amplitude).x) * wbal;
				}

				noise.push_back(val);
				
				//A Output
				noise_gen.setSeed(m_seed * 4);
				if (wbal > 0)
				{
					val = (1.0f - noise_gen.brownianTiledVoronoi(pos * 3 / division_amount, m_noise.m_vor_octaves,
						m_noise.m_vor_frequency, m_noise.m_vor_gain, m_noise.m_vor_lacunarity, m_noise.m_vor_amplitude).x) * wbal;
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
