#include "SceneSelect.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "Lighting.h"
#include "Terrain.h"
#include "VectorToArray.h"
#include "Texture.h"
#include "ComputeShader.h"


bool SceneSelect::m_popup_toggle = true;

SceneSelect::SceneSelect(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vector3D(8, 7, -6));

	m_tex1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\Env.png");

	//if (m_first_time) m_first_time = false;



	makeComputeShaderTextureTest();



	

}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::update(float delta, const float& width, const float& height)
{
}

void SceneSelect::imGuiRender()
{
	//if (ImGui::Button("Explanation")) m_popup_toggle = true;
	//if (m_popup_toggle)
	//{
	//	ImGui::SetNextWindowSize(ImVec2(400, 400));
	//	Vector2D size = AppWindow::getScreenSize();

	//	ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));

		ImTextureID t = m_srv;/* Lets make a shader resource view out of the image data we generated and display it here */;

	//	ImGui::OpenPopup("Welcome");
	//	ImGui::BeginPopupModal("Welcome");
		ImGui::Image(t, ImVec2(300, 300));
	//	if (ImGui::Button("Okay", ImVec2(100, 30))) m_popup_toggle = false;
	//	ImGui::EndPopup();
	//}

	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(1000, 700));
	ImGui::SetNextWindowPos(ImVec2(0, 20));

	//create the test window
	ImGui::Begin("Scene Selection");
	ImGui::Text("Press 1 key to display the mouse");
	if (ImGui::Button("Shaders", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE01, true);
	if (ImGui::Button("ShadowMapping", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE02, true);
	if (ImGui::Button("Terrain Generator", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE03, true);
	if (ImGui::Button("Dynamic Terrain", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE04, true);
	if (ImGui::Button("Tesselation", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE05, true);
	if (ImGui::Button("Character Animation", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE06, true);
	if (ImGui::Button("Noise", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE07, true);
	if (ImGui::Button("Volumetric Clouds", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE08, true);
	if (ImGui::Button("Weather Map", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE09, true);
	if (ImGui::Button("Character Movement", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE10, true);
	if (ImGui::Button("Stage Creator", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE11, true);
	if (ImGui::Button("Stage Example", ImVec2(980, 30))) p_manager->changeScene(SceneManager::SCENE12, true);


	ImGui::End();
}

void SceneSelect::makeComputeShaderTextureTest()
{

	//temporary function for testing compute shader functionality

	//lets make a 32 * 32 rgba image
	UINT pixelcount = 32 * 32 * 4;
	//Vector4D image_data[32 * 32 * 4] = {};
	std::vector<Vector4D> listpixels;
	listpixels.resize(pixelcount);


	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"ComputeTest.hlsl", "CS_main", &shader_byte_code, &size_shader);
	m_cs = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader, sizeof(Vector4D), 
		sizeof(Vector4D), &listpixels[0], pixelcount);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_cs->setXDispatchCount(pixelcount);
	m_cs->setYDispatchCount(1);

	int a = 3;

	compute_image_data = reinterpret_cast<Vector3D*>(m_cs->runComputeShader());

	memcpy(&listpixels[0], compute_image_data, pixelcount * sizeof(Vector4D));

	//lets create a shader resource view here to store and try displaying in ImGui.
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = 32;
	desc.Height = 32;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;



	const UINT bytesPerPixel = 16;
	//UINT sliceSize = cWidth * cHeight * bytesPerPixel;

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = compute_image_data;
	initData.SysMemPitch = 32 * bytesPerPixel;
	initData.SysMemSlicePitch = 32 * 32 * bytesPerPixel;
	//initData.SysMemSlicePitch = cWidth * cHeight * bytesPerPixel;


	ID3D11Texture2D* tex = nullptr;
	HRESULT hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&desc, &initData, &tex);
	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(tex, NULL, &m_srv);


	m_cs->unmapCPUReadable();
}

void SceneSelect::shadowRenderPass(float delta)
{
}

void SceneSelect::mainRenderPass(float delta)
{
}