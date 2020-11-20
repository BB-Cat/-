#include "Scene13.h"
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
#include "Texture.h"
#include "Cube.h"
#include "WorldObjectManager.h"
#include "ActorManager.h"
#include "ComputeShader.h"
#include "Terrain.h"
#include "Vector2D.h"

#include <iostream>
#include <fstream>  



//bool Scene03::m_first_time = true;

Scene13::Scene13(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::FREE);
	CameraManager::get()->setCamPos(Vector3D(4, 40, -15));
	CameraManager::get()->setCamRot(Vector2D(0.4, 0));
	CameraManager::get()->setSpeed(10);

	Lighting::get()->updateSceneLight(Vector3D(0.4, 0.6, 0), Vector3D(1, 1, 0.8), 1.0f, Vector3D(0.1, 0.1, 0.4));


	UINT pixelcount = 512 * 512 * 4;
	std::vector<Vector4D> listpixels;
	listpixels.resize(pixelcount);

	//compile the test compute shader
	//void* shader_byte_code = nullptr;
	//size_t size_shader = 0;
	//GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"ComputeTest.hlsl", "CS_main", &shader_byte_code, &size_shader);
	//m_cs = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader, sizeof(Vector4D),
	//	sizeof(Vector4D), &listpixels[0], pixelcount);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//makeComputeShaderTexture();

	getComputerShaderVertexList();
	initTerrainBuffers();
	initRasterizers();
	//m_terrain = std::make_shared<Terrain>(new Terrain("..\\Assets\\map.bmp", "..\\Assets\\texturesplat.bmp"));
	//TerrainPtr t(new Terrain("..\\Assets\\map.bmp", "..\\Assets\\texturesplat.bmp", Vector2D(0, 0)));
	std::vector<VertexMesh> temp;
	temp.resize(33 * 33);
	for (int i = 0; i < 1024; i++)
	{
		memcpy(&temp[0], &m_verts[33 * 33 * i], 33 * 33 * sizeof(VertexMesh));
		TerrainPtr t(new Terrain(temp));
		m_terrain[i] = t;
		temp.clear();
		temp.resize(33 * 33);
	}
	//TerrainPtr t(new Terrain(m_verts));
	//m_terrain = t;
	initTextures();
}

Scene13::~Scene13()
{
	m_rs->Release();
	m_rs2->Release();
	
}

void Scene13::update(float delta, const float& width, const float& height)
{
	CameraManager::get()->update(delta, width, height);
	m_timer++;
}

void Scene13::imGuiRender()
{
	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	ImGui::SetNextWindowSize(ImVec2(300, 800));
	ImGui::SetNextWindowPos(ImVec2(0, 20));



	//create the test window
	ImGui::Begin("Scene Settings");
	ImGui::Text("Press 1 key to display the mouse");

	//ImTextureID t = m_srv;/* Lets make a shader resource view out of the image data we generated and display it here */;
	//ImGui::Image(t, ImVec2(300, 300));

	if (ImGui::Button("Scene Select")) p_manager->changeScene(SceneManager::SCENESELECT, false);

	if(ImGui::Button("Show Wireframe")) m_show_wire = !m_show_wire;
	if (ImGui::Button("Show Normals")) m_active_shader = Shaders::TERRAIN_TEST;
	if (ImGui::Button("HD Shader")) m_active_shader = Shaders::TERRAIN_HD_TOON;
	if (ImGui::Button("LD Shader")) m_active_shader = Shaders::TERRAIN_LD_TOON;
	//VectorToArray v(&m_global_light_rotation);
	//ImGui::DragFloat2("Light Direction", v.setArray(), 0.02f, -6.28f, 6.28f);


	//v = VectorToArray(&m_light_color);
	//ImGui::DragFloat3("Light Color", v.setArray(), 0.01f, 0, 1.0);
	//ImGui::DragFloat("Light Strength", &m_global_light_strength, 0.01f, 0, 1.0);

	//v = VectorToArray(&m_ambient_light_color);
	//ImGui::DragFloat3("Ambient Color", v.setArray(), 0.01f, 0, 1.0);


	//WorldObjectManager::get()->imGuiRender();

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));

		ImGui::OpenPopup("Compute Shader Popup");
		ImGui::BeginPopupModal("Compute Shader Popup");
		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();
}

void Scene13::shadowRenderPass(float delta)
{
}

void Scene13::mainRenderPass(float delta)
{
	Vector3D campos = CameraManager::get()->getCamera().getTranslation();
	GraphicsEngine::get()->getShaderManager()->setPipeline(m_active_shader);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_hd);


	//set all textures for the terrain
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture3SplatTex(
		m_tex1, m_displace1_1, m_norm1, m_rough1, m_ambient_occ1,
		m_tex2, m_displace2_1, m_norm2, m_rough2, m_ambient_occ2,
		m_tex3, m_displace3_1, m_norm3, m_rough3, m_ambient_occ3,
		m_tex4, m_displace4_1, m_norm4, m_rough4, m_ambient_occ4);


	if(!m_show_wire) GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_rs);
	else GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_rs2);
	//m_terrain->render(0, 0, 0);
	for (int i = 0; i < 1024; i++)
	{
		m_terrain[i]->render(0, 0, 0);
	}
}

void Scene13::makeComputeShaderTexture()
{
//temporary function for testing compute shader functionality
	m_cs->setXDispatchCount(256);
	m_cs->setYDispatchCount(1);
	m_cs->runComputeShader();

	m_srv = m_cs->createTextureSRVFromOutput(Vector2D(512, 512));

	m_cs->unmapCPUReadable();
}

void Scene13::getComputerShaderVertexList()
{
	//compile the test compute shader
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"ComputeHeightmap.hlsl", "CS_main", &shader_byte_code, &size_shader);
	m_cs = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader, sizeof(Vector4D),
		sizeof(VertexMesh), nullptr, 33 * 33 * 1024);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_cs->setXDispatchCount(33792);
	m_cs->setYDispatchCount(1);

	high_resolution_timer t;
	t.start();

	m_cs->runComputeShader();



	
	m_verts.resize(33 * 33 * 1024);
	VertexMesh* temp;
	temp = reinterpret_cast<VertexMesh*>(m_cs->getOutputData());

	memcpy(&m_verts[0], m_cs->getOutputData(), 33 * 33 * sizeof(VertexMesh) * 1024);

	t.tick();
	float value = t.time_interval();

	m_cs->unmapCPUReadable();

	//high_resolution_timer t2;
	//t2.start();

	//TerrainPtr terr(new Terrain("..\\Assets\\map.bmp", "..\\Assets\\texturesplat.bmp", Vector2D(1, 1)));
	//TerrainPtr terr2(new Terrain("..\\Assets\\map.bmp", "..\\Assets\\texturesplat.bmp", Vector2D(1, 1)));

	//t2.tick();
	//float value2 = t2.time_interval();
	//int a = 3;

}

void Scene13::initTerrainBuffers()
{
	//initialize the index buffers

	int cols = 33;
	int rows = 33;

	//Create the grid
	//m_num_vertexes = rows * cols;
	int num_faces = (rows - 1) * (cols - 1) * 2;

	std::vector<DWORD> indices(num_faces * 3);

	int k = 0;
	for (DWORD i = 0; i < rows - 1; i++)
	{
		for (DWORD j = 0; j < cols - 1; j++)
		{
			indices[k] = i * cols + j;        // Bottom left of quad

			indices[k + 1] = i * cols + j + 1;        // Bottom right of quad

			indices[k + 2] = (i + 1) * cols + j;    // Top left of quad


			indices[k + 3] = (i + 1) * cols + j;    // Top left of quad

			indices[k + 4] = i * cols + j + 1;        // Bottom right of quad

			indices[k + 5] = (i + 1) * cols + j + 1;    // Top right of quad

			k += 6; // next quad

		}
	}

	//std::vector<DWORD> indices2((m_num_faces * 3)/3);
	//std::vector<DWORD> indices2(m_num_faces / 4 * 3);
	std::vector<DWORD> indices2((num_faces * 3 / 4));

	k = 0;
	//texUIndex = 0;
	//texVIndex = 0;

	for (DWORD i = 0; i < rows - 1; i += 2)
	{
		for (DWORD j = 0; j < cols - 1; j += 2)
		{
			indices2[k] = i * cols + j;        // Bottom left of quad

			indices2[k + 1] = i * cols + j + 2;        // Bottom right of quad

			indices2[k + 2] = (i + 2) * cols + j;    // Top left of quad

			indices2[k + 3] = (i + 2) * cols + j;    // Top left of quad

			indices2[k + 4] = i * cols + j + 2;        // Bottom right of quad

			indices2[k + 5] = (i + 2) * cols + j + 2;    // Top right of quad

			k += 6; // next quad
		}

	}

	std::vector<DWORD> indices3((num_faces * 3 / 16));

	k = 0;

	for (DWORD i = 0; i < rows - 1; i += 4)
	{
		for (DWORD j = 0; j < cols - 1; j += 4)
		{
			indices3[k] = i * cols + j;        // Bottom left of quad

			indices3[k + 1] = i * cols + j + 4;        // Bottom right of quad

			indices3[k + 2] = (i + 4) * cols + j;    // Top left of quad

			indices3[k + 3] = (i + 4) * cols + j;    // Top left of quad

			indices3[k + 4] = i * cols + j + 4;        // Bottom right of quad

			indices3[k + 5] = (i + 4) * cols + j + 4;    // Top right of quad

			k += 6; // next quad

		}
	}


	m_hd = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());
	m_md = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices2[0], (UINT)indices2.size());
	m_ld = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices3[0], (UINT)indices3.size());
}

void Scene13::initRasterizers()
{
	HRESULT hr;

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rsDesc, &m_rs);
	if (FAILED(hr))	assert(0 && "Error loading the fill terrain rasterizer");

	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rsDesc, &m_rs2);
	if (FAILED(hr))	assert(0 && "Error loading the wire terrain rasterizer");
}

void Scene13::initTextures()
{
	m_tex1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Basecolor_A.png");
	m_displace1_1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Height.png");
	m_norm1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Normal.png");
	m_rough1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Metallic.png");
	m_ambient_occ1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_AO.png");


	m_tex2 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Basecolor.png");
	m_displace2_1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Height.png");
	m_norm2 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Normal.png");
	m_rough2 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Metallic.png");
	m_ambient_occ2 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_AO.png");

	m_tex3 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Basecolor.png");
	m_displace3_1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Height.png");
	m_norm3 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Normal.png");
	m_rough3 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Metallic.png");
	m_ambient_occ3 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_AO.png");

	m_tex4 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Basecolor_A.png");
	m_displace4_1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Height.png");
	m_norm4 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Normal.png");
	m_rough4 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Metallic.png");
	m_ambient_occ4 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_AO.png");

}

