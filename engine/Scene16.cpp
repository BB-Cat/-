#include "Scene16.h"
#include "AppWindow.h"
#include "CameraManager.h"
#include "Lighting.h"
#include "ConstantBufferSystem.h"
#include "Sprite.h"
#include "Texture3D.h"
#include "DeviceContext.h"
#include "Texture.h"
#include "ComputeShader.h"
#include "TextureComputeShader.h"


#include <iostream>
#include <fstream>  



//bool Scene03::m_first_time = true;

Scene16::Scene16(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::FREE);
	CameraManager::get()->setCamPos(Vec3(0, 1, 0));
	CameraManager::get()->setSpeed(0.5f);

	sky = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\SkyBoxTex\\cape_hill.jpg");



	m_global_light_rotation = Vec2(70 * 0.01745f, 70 * 0.01745f);
	m_global_light_strength = 0.85f;
	m_light_color = Vec3(0.4, 0.6, 0.0);
	m_ambient_light_color = Vec3(0.4, 0.3, 0.4);

	Vec2 screensize = AppWindow::getScreenSize();
	m_raymarch_buffer.m_screensize = Vector4D(screensize.x, screensize.y, 0, 0);
	m_raymarch_buffer.m_params.m_x = 1;
	m_raymarch_buffer.m_params.m_y = 6;

	m_raymarch_buffer.m_pointlight_pos = Vec3(3, 7, -1);
	m_raymarch_buffer.m_ground_color = Vec3(0.5, 0.4, 0.2);
	generateSpheres();



	//initSpriteTexture();

	UINT pixelcount = screensize.x * screensize.y * 4;
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"ComputeRayMarchSpheres.hlsl", "CS_main", &shader_byte_code, &size_shader);
	m_compute_raymarch = GraphicsEngine::get()->getRenderSystem()->createTextureComputeShader(shader_byte_code, size_shader, screensize);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetCSRaymarchBuffer(m_raymarch_buffer);
	if(m_compute_raymarch != nullptr) makeComputeShaderTexture();

}

Scene16::~Scene16()
{

}

void Scene16::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->update(delta, width, height);
	if (!m_stop_update) m_timer += delta;
	updateSphereData();
	
	
	if (m_compute_raymarch != nullptr) makeComputeShaderTexture();

}

void Scene16::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(215, 45));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, 65));
	ImGui::SetNextWindowBgAlpha(0.6f);
	
	ImGui::Begin("Ray Tracing", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::SliderInt("Traces", &m_tracecount, 1, 5);
	ImGui::SliderInt("Spheres", &m_spherecount, 0, 12);

	if (ImGui::Button("Start/Stop Movement", ImVec2(200, 20))) m_stop_update = !m_stop_update;

	if (ImGui::Button("Rearrange Spheres", ImVec2(200, 20))) generateSpheres();

	m_raymarch_buffer.m_params.m_x = m_tracecount;
	m_raymarch_buffer.m_params.m_y = m_spherecount;

	ImGui::End();

}

void Scene16::shadowRenderPass(float delta)
{
}

void Scene16::mainRenderPass(float delta)
{
	sprite->renderScreenSpaceSprite(Vec2(1,1), Vec2(0,0), 0, Vec2(0, 0), AppWindow::getScreenSize(), Vec2(0, 0));

}

void Scene16::initSpriteTexture()
{
	//lets create a shader resource view here to store and try displaying in ImGui.
	D3D11_TEXTURE2D_DESC desc{};
	Vec2 screensize = AppWindow::getScreenSize();
	desc.Width = screensize.x;
	desc.Height = screensize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;



	const UINT bytesPerPixel = 16;
	//UINT sliceSize = cWidth * cHeight * bytesPerPixel;

	std::vector<float> init_tex;
	init_tex.resize(screensize.x * screensize.y * 4);

	/*memset(&initdata[0], 1, sizeof(float) * 4 * initdata.size());*/

	//initialize the texture to be pure black
	for (size_t i = 0; i + 3 < init_tex.size();) 
	{
		init_tex[i] = 1.0f;
		init_tex[i + 1] = 0.0f;
		init_tex[i + 2] = 0.0f;
		init_tex[i + 3] = 1.0f;

		i += 4;
	}

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = &init_tex[0];
	initData.SysMemPitch = screensize.x * bytesPerPixel;
	initData.SysMemSlicePitch = screensize.x * screensize.y * bytesPerPixel;


	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* temp;
	//HRESULT hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&desc, &initData, &tex);

	HRESULT hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&desc, &initData, &tex);
	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(tex, NULL, &temp);
	tex->Release();
	init_tex.clear();


	TexturePtr texptr = std::make_shared<Texture>(temp, screensize.x, screensize.y);
	sprite = std::make_shared<Sprite>(texptr);
}

void Scene16::makeComputeShaderTexture()
{
	//temporary function for testing compute shader functionality
	Vec2 screensize = AppWindow::getScreenSize();
	int numthreadsx = 1024;

	int dispatch_count = screensize.x * screensize.y / numthreadsx;

	TextureComputeShader* temp;

	m_compute_raymarch->setXDispatchCount(dispatch_count);
	m_compute_raymarch->setYDispatchCount(1);


	//set the shader
	m_compute_raymarch->setComputeShader();

	//apply texture
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTextureCS(sky);
	
	//run
	//m_compute_raymarch->runComputeShader();
	m_compute_raymarch->runComputeShaderNoSet();

	if (sprite == nullptr)
	{
		TexturePtr texptr = std::make_shared<Texture>(m_compute_raymarch->getTexture(), screensize.x, screensize.y);
		sprite = std::make_shared<Sprite>(texptr);
	}
	else
	{
		sprite->getTexture()->InitGeneratedTexture(
			m_compute_raymarch->getTexture(),
			screensize.x,
			screensize.y);
	}

	//	if (sprite == nullptr)
	//{
	//	TexturePtr texptr = std::make_shared<Texture>(m_compute_raymarch->createTextureSRVFromOutput(
	//		Vec2(screensize.x, screensize.y)), screensize.x, screensize.y);
	//	sprite = std::make_shared<Sprite>(texptr);
	//}
	//	else
	//	{
	//		sprite->getTexture()->InitGeneratedTexture(
	//			m_compute_raymarch->createTextureSRVFromOutput(Vec2(screensize.x, screensize.y)),
	//			screensize.x,
	//			screensize.y);
	//	}

}

void Scene16::generateSpheres()
{
	int max_spheres = 12;

	//generate randum radii
	float radii[12];
	for (int i = 0; i < max_spheres; i++) radii[i] = rand() % 50 / 100.0f + 0.5f;

	Vec2 pos[12];
	Vec2 newpos;
	float rangebias = 0.2f;
	//generate positions that dont collide with eachother.
	for (int i = 0; i < max_spheres; i++)
	{
		newpos = Vec2((rand() % 100 / 100.0f - 0.5f) * rangebias, rand() % 100 / 100.0f * rangebias);

		//make sure it's not colliding with other spheres, otherwise we loop and increase the range bias slightly
		bool check = false;
		for (int j = 0; j < i; j++) if ((pos[j] - newpos).length() < radii[j] + radii[i]) check = true;
		if (check)
		{
			i--;
			rangebias += 0.1f;
		}
		else pos[i] = newpos;
	}

	for (int i = 0; i < max_spheres; i++)
	{
		m_raymarch_buffer.m_sphere_pos_and_radius[i].m_x = pos[i].x;
		m_raymarch_buffer.m_sphere_pos_and_radius[i].m_z = pos[i].y + 1.5;

		m_raymarch_buffer.m_sphere_pos_and_radius[i].m_w = radii[i]; //radius from 0.5 to 1.0
		m_raymarch_buffer.m_sphere_pos_and_radius[i].m_y = radii[i] + 0.01; //height from radius
	
		m_raymarch_buffer.m_local_speculars[i] = Vector4D(0.6, 0.6, 0.6, 0);

		if (rand() % 3)
		{
			m_raymarch_buffer.m_local_colors[i] = Vector4D(rand() % 10 / 10.0f, rand() % 10 / 10.0f, rand() % 10 / 10.0f, 1) * 0.7f + Vector4D(0.3, 0.3, 0.3, 1.0);
			m_raymarch_buffer.m_local_speculars[i] = m_raymarch_buffer.m_local_speculars[i] * 
				m_raymarch_buffer.m_local_colors[i];

			if (rand() % 2) m_raymarch_buffer.m_local_speculars[i] = m_raymarch_buffer.m_local_speculars[i] * 0.3f;
		}


	
	}
}

void Scene16::updateSphereData()
{

	cb_compute_raymarch temp = m_raymarch_buffer;
	m_raymarch_buffer.m_pointlight_pos = Vec3(sinf(m_timer) * 5, 5.0f, cosf(m_timer)) * 5 + 5;
	Vec3 dir = m_raymarch_buffer.m_pointlight_pos.xyz() * -1;
	m_raymarch_buffer.m_light_dir = dir.getNormalized();
	m_raymarch_buffer.m_light_dir.m_w = 1.0;
	
	int max_spheres = 12;
	for (int i = 0; i < max_spheres; i++)
	{
		temp.m_sphere_pos_and_radius[i].m_y = m_raymarch_buffer.m_sphere_pos_and_radius[i].m_y + 
			abs(sinf(m_timer * temp.m_sphere_pos_and_radius[i].m_w * 3));
	}

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetCSRaymarchBuffer(temp);
}
