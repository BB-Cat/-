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
#include "WorldObject.h"
#include "WorldObjectManager.h"
#include "ActorManager.h"
#include "ComputeShader.h"
#include "Terrain.h"
#include "Vector2D.h"
#include "TerrainManager.h"
#include "TextureComputeShader.h"

#include <iostream>
#include <fstream>  



//bool Scene03::m_first_time = true;

Scene13::Scene13(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(CAMERA_STATE::FREE);
	CameraManager::get()->setCamPos(Vec3(4, 40, -15));
	CameraManager::get()->setCamRot(Vec2(0.4, 0));
	CameraManager::get()->setSpeed(2);

	Lighting::get()->updateSceneLight(Vec3(0.4, 0.6, 0), Vec3(1, 1, 0.8), 1.0f, Vec3(0.1, 0.1, 0.4));






	//UINT pixelcount = 512 * 512 * 4;
	//std::vector<Vector4D> listpixels;
	//listpixels.resize(pixelcount);

	////////////////////////////////
	//compile the compute shaders //

	//UINT pixelcount = 512 * 512 * 4;
	//void* shader_byte_code = nullptr;
	//size_t size_shader = 0;
	//GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"ComputeNoiseTex.hlsl", "CS_main", &shader_byte_code, &size_shader);
	//m_compute_noisetex = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader, sizeof(Vector4D),
	//	sizeof(Vector4D), nullptr, pixelcount);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	Vec2 imagesize = Vec2(512, 512);
	UINT pixelcount = imagesize.x * imagesize.y * 4;
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"ComputeNoiseTex.hlsl", "CS_main", &shader_byte_code, &size_shader);
	m_compute_noisetex = GraphicsEngine::get()->getRenderSystem()->createTextureComputeShader(shader_byte_code, size_shader, imagesize);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	initTerrainBuffers();
	initRasterizers();
	initTextures();
	initNoiseBuffer();

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetCSNoiseBuffer(m_noise);
	makeComputeShaderTexture();
}

Scene13::~Scene13()
{
	m_rs->Release();
	m_rs2->Release();
	if (m_srv != nullptr) m_srv->Release();
	if (m_tman) delete m_tman;
	m_tman = nullptr;
	
}

void Scene13::update(float delta)
{
	CameraManager::get()->update(delta);

	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetCSNoiseBuffer(m_noise);

	//this is a hotfix for an issue where the d3ddevice is released when the texture compute shader makes a new SRV right before making new terrain
	if (m_create_terrain_timer == -1)
	{
		if(m_change) makeComputeShaderTexture();
	}
	else if (m_create_terrain_timer) m_create_terrain_timer--;
	else if (m_create_terrain_timer == 0)
	{
		CameraManager::get()->setCamPos(Vec3(9.5, 0, 9.5) * 99 + Vec3(0, 300, 0));
		m_tman = new TerrainManager(Vec2(17, 17));
		m_create_terrain_timer--;
	}


	m_timer++;
}

void Scene13::imGuiRender()
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

	ImGui::SetNextWindowPos(ImVec2(215, 20));
	ImGui::SetNextWindowBgAlpha(0.2f);

	ImGui::Begin("Explanation", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);
	ImGui::Text("Press ESC key to display the mouse or control the camera");
	ImGui::End();




	ImVec2 size = ImVec2(200, 20);

	if (m_create_terrain_timer == -1)
	{
		ImGui::SetNextWindowPos(ImVec2(700, 20));
		//create the test window
		ImGui::SetNextWindowBgAlpha(0.6f);
		ImGui::Begin("TexDisplay", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

		if (m_srv != nullptr)
		{
			ImTextureID t = m_srv;/* Lets make a shader resource view out of the image data we generated and display it here */;
			ImGui::Image(t, ImVec2(300, 300));
		}

		ImGui::End();
	}

	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowPos(ImVec2(610, 555));
	ImGui::SetNextWindowSize(ImVec2(410, 200));

	ImGui::Begin("Scene Settings", 0, ImGuiWindowFlags_NoTitleBar);
	
	ImGui::Text("Noise settings");

	if (ImGui::CollapsingHeader("Perlin Noise"))
	{
		ImGui::PushID("Perlin");
		if (ImGui::DragInt("Octaves", &m_int_per_octave, 0.05f, 0.00f, 5.0f)) m_change = true;
		m_noise.m_per_octaves = m_int_per_octave;
		if(ImGui::DragFloat("Amplitude", &m_noise.m_per_amplitude, 0.005f, 0.00f, 5.0f)) m_change = true;
		if(ImGui::DragFloat("Gain", &m_noise.m_per_gain, 0.005f, -10.00f, 10.0f)) m_change = true;
		if(ImGui::DragFloat("Lacunarity", &m_noise.m_per_lacunarity, 0.005f, -10.00f, 10.0f)) m_change = true;
		if(ImGui::DragFloat("Perlin Cell Size", &m_noise.m_per_cell_size, 0.01f, 0.001f)) m_change = true;
		ImGui::PopID();
	}
	else if (ImGui::CollapsingHeader("Ridged Fractal Noise"))
	{
		ImGui::PushID("Ridged");
		if(ImGui::DragInt("Octaves", &m_int_ridge_per_octave, 0.05f, 0.00f, 5.0f)) m_change = true;
		m_noise.m_ridged_per_octaves = m_int_ridge_per_octave;
		if(ImGui::DragFloat("Amplitude", &m_noise.m_ridged_per_amplitude, 0.005f, 0.00f, 5.0f)) m_change = true;
		if(ImGui::DragFloat("Gain", &m_noise.m_ridged_per_gain, 0.005f, -10.00f, 10.0f)) m_change = true;
		if(ImGui::DragFloat("Lacunarity", &m_noise.m_ridged_per_lacunarity, 0.005f, -10.00f, 10.0f)) m_change = true;
		if(ImGui::DragFloat("Ridge Cell Size", &m_noise.m_ridged_per_cell_size, 0.01f, 0.001f)) m_change = true;
		ImGui::PopID();
	}
	else
	{
		if(ImGui::InputFloat("Set Seed", &m_noise.m_seed)) m_change = true;
		//if(ImGui::DragFloat("Cell Size", &m_noise.m_compute_cell_size, 0.01f, 0.001f)) m_change = true; //float m_compute_cell_size;
		VectorToArray v(&m_noise.m_noise_type);
		float* vals = v.setArray();
		ImGui::Text("Perlin Concentration");
		if(ImGui::SliderFloat(" ", &vals[0], 0, 1.0f, "%.2f")) m_change = true;
		ImGui::Text("Ridged Concentration");
		if (ImGui::SliderFloat("  ", &vals[2], 0, 1.0f, "%.2f")) m_change = true;
	}
	//if (ImGui::Button("LD Shader")) m_active_shader = Shaders::TERRAIN_LD_TOON;

	if (m_first_time)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));

		ImGui::OpenPopup("Compute Shader Popup");
		ImGui::BeginPopupModal("Compute Shader Popup");
		//ImGui::Image(t, ImVec2(300, 300));
		ImGui::TextWrapped("This scene generates terrain using the GPU and noise functions.  Change the noise settings to something interesting, then press 'Test Terrain.'");
		ImGui::NewLine();
		ImGui::TextWrapped("If you move around, new terrain will be created.  Sometimes there will be a mistake when loading.  I need to adjust my thread code which was changed recently, and then the issue will be fixed.");
		
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_first_time = false;
		ImGui::EndPopup();
	}

	ImGui::End();

	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowPos(ImVec2(800, 340));
	//ImGui::SetNextWindowSize(ImVec2(310, 160));

	ImGui::Begin("Scene Control", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	
	if (m_tman == nullptr)
	{
		if (ImGui::Button("Test Terrain", size))
		{

			//for some reason, having the shader resource view active when initializing another computer shader causes DX11 to crash.
			//I need to research this more.  In order to prevent this crashing we are setting a timer with this button, which will load
			//new terrain when it hits 0, giving the shader resource view time to stop being used.
			if(m_create_terrain_timer == -1) m_create_terrain_timer = 4;
			if (m_srv)
			{
				m_srv->Release();
				m_srv = nullptr;
			}
			
		}
		//if (ImGui::Button("Test New Chunk"))
		//{
		//	if (m_tman) m_tman->onNewChunkCompute(Vector2D(1, 0));
		//}
	}
	else 
	{
		if (ImGui::Button("Show Wireframe", size)) m_show_wire = !m_show_wire;
		if (ImGui::Button("Show Normals", size)) m_active_shader = Shaders::TERRAIN_TEST;
		if (ImGui::Button("LOD Shader", size)) m_active_shader = -1;

		ImGui::NewLine();
		if (ImGui::Button("Reset Terrain", size))
		{
			delete m_tman;
			m_tman = nullptr;
		}
	}

	ImGui::End();
}

void Scene13::shadowRenderPass(float delta)
{
}

void Scene13::mainRenderPass(float delta)
{
	Vec3 campos = CameraManager::get()->getCamera().getTranslation();

	if (m_tman != nullptr)
	{
		//m_tman->update();
		m_tman->render(m_active_shader, 1.0, m_show_wire, 0);
	}
}

void Scene13::makeComputeShaderTexture()
{
	////temporary function for testing compute shader functionality
	//m_compute_noisetex->setXDispatchCount(256);
	//m_compute_noisetex->setYDispatchCount(1);
	//m_compute_noisetex->runComputeShader();

	////make sure we are releasing data properly
	//if (m_srv != nullptr) m_srv->Release();
	//m_srv = m_compute_noisetex->createTextureSRVFromOutput(Vec2(512, 512));


	////m_compute_noisetex->createTextureSRVFromOutput(Vector2D(512, 512));

	////m_compute_noisetex->unmapCPUReadable();

		//temporary function for testing compute shader functionality
	Vec2 texsize = Vec2(512, 512);
	int numthreadsx = 1024;

	int dispatch_count = texsize.x * texsize.y / numthreadsx;

	TextureComputeShader* temp;

	m_compute_noisetex->setXDispatchCount(256);
	m_compute_noisetex->setYDispatchCount(1);

	//set the shader
	m_compute_noisetex->setComputeShader();

	//m_compute_raymarch->runComputeShader();
	m_compute_noisetex->runComputeShaderNoSet();

	if (m_srv != nullptr) m_srv->Release();
	m_srv = m_compute_noisetex->getTexture();
}

void Scene13::runTerrainComputeShader()
{
	m_compute_terrain->setXDispatchCount(34);//33792);
	m_compute_terrain->setYDispatchCount(1);

	high_resolution_timer t;
	t.start();

	m_compute_terrain->runComputeShader();


	
	m_verts.resize(34 * 34 * 1024);
	VertexMesh* temp;
	temp = reinterpret_cast<VertexMesh*>(m_compute_terrain->getOutputData());

	memcpy(&m_verts[0], m_compute_terrain->getOutputData(), 34 * 34 * sizeof(VertexMesh) * 1024);

	t.tick();
	float value = t.time_interval();

	m_compute_terrain->unmapCPUReadable();
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

	int total_verts = 33 * 2 + 2; //this is the total amount of vertice data available in each strip

	int verts_high = SEAMLESS_CHUNK;
	int verts_mid = (SEAMLESS_CHUNK) / 2;
	int verts_low = (SEAMLESS_CHUNK) / 4;

	//Create the grid
	int num_faces_MH = verts_high + verts_mid + 2; //2 additional vertexes for the cap at both ends of the seam
	int num_faces_LM = verts_mid + verts_low + 2;



	//THESE INDEX BUFFERS ARE UPDATED FOR THE COMPUTE SHADER TERRAIN GENERATOR!

	//===============================================================================
	//  HIGH INDEX
	//===============================================================================

	indices.resize(66 * 3); //64 faces in a high res seam plus two additional faces for the space between seams


	indices[0] = 66;
	indices[1] = 67;
	indices[2] = 0;

	k = 3;

	for (int i = 1; i <= verts_high; i++)
	{
		indices[k] = i + 1;
		indices[k + 1] = i + 34;
		indices[k + 2] = i;

		k += 3;
	}

	for (int i = 1; i <= verts_high; i++)
	{
		indices[k] = i + 33 + 1;
		indices[k + 1] = i + 33;
		indices[k + 2] = i;

		k += 3;
	}

	//create the index for the bottom cap
	indices[k] =     67;
	indices[k + 2] = 33;
	indices[k + 1] = 66;

	m_LOD_seam_high = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

	//===============================================================================
	//===============================================================================

	//===============================================================================
	//  MID INDEX
	//===============================================================================

	indices.clear();
	indices.resize(34 * 3); //32 faces in a high res seam plus two faces for caps

	//create the index for the top cap
	indices[0] = 66;
	indices[1] = 67;
	indices[2] = 0;

	k = 3;

	for (int i = 1; i <= verts_mid; i++)
	{
		indices[k] = (i + 1) * 2 - 1;
		indices[k + 1] = i * 2 - 1 + 33;
		indices[k + 2] = i * 2 - 1;

		k += 3;
	}

	for (int i = 1; i <= verts_mid; i++)
	{
		indices[k] = i * 2 - 1 + 35;
		indices[k + 1] = i * 2 - 1 + 33;
		indices[k + 2] = (i + 1) * 2 - 1;

		k += 3;
	}

	//create the index for the bottom cap
	indices[k] = 67;
	indices[k + 2] = 33;
	indices[k + 1] = 66;

	m_LOD_seam_mid = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

	//===============================================================================
	//===============================================================================

	//===============================================================================
	//  LOW INDEX
	//===============================================================================

	indices.clear();
	indices.resize(18 * 3); //16 faces in a high res seam plus two faces for caps

	//create the index for the top cap
	indices[0] = 66;
	indices[1] = 67;
	indices[2] = 0;

	k = 3;

	for (int i = 1; i <= verts_low; i++)
	{
		indices[k] = i * 4 + 1;
		indices[k + 1] = (i - 1) * 4 + 34;
		indices[k + 2] = (i - 1) * 4 + 1;


		k += 3;
	}

	for (int i = 1; i <= verts_low; i++)
	{
		indices[k] = i * 4 + 34;
		indices[k + 1] = i * 4 + 1;
		indices[k + 2] = (i - 1) * 4 + 34;


		k += 3;
	}

	//create the index for the bottom cap
	indices[k] = 67;
	indices[k + 2] = 33;
	indices[k + 1] = 66;

	m_LOD_seam_low = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

	//===============================================================================
	//===============================================================================



	//===============================================================================
	//  HIGH TO MID INDEX
	//===============================================================================

	indices.clear();
	indices.resize(num_faces_MH * 3);

	//create the index for the top cap
	indices[0] = 66;
	indices[1] = 67;
	indices[2] = 0;


	k = 3;
	//create the high resolution half of the index, with a number of triangles equal to the edge vertexes of a mid resolution chunk
	for (int i = 1; i <= verts_high; i++)
	{
		indices[k] = i + 1;
		indices[k + 1] = (i / 2) * 2 + 34;
		indices[k + 2] = i;


		k += 3;
	}
	//create the second half of the index, with a number of triangles equal to the edge vertexes of a low resolution chunk
	for (int i = 1; i <= verts_mid; i++)
	{
		indices[k] = i * 2 + 34;
		indices[k + 1] = (i - 1) * 2 + 34;
		indices[k + 2] = i * 2;


		k += 3;
	}

	//create the index for the bottom cap
	indices[k] = 67;
	indices[k + 2] = 33;
	indices[k + 1] = 66;

	m_LOD_highToMid = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

	//===============================================================================
	//===============================================================================

	//===============================================================================
	//  MID TO HIGH INDEX
	//===============================================================================
		/* This is essentially the same as HIGH to MID index, we just reverse the order */
	indices.clear();
	indices.resize(num_faces_MH * 3);

	//create the index for the top cap
	indices[0] = 66;
	indices[1] = 67;
	indices[2] = 0;


	k = 3;
	//create the high resolution half of the index, with a number of triangles equal to the edge vertexes of a mid resolution chunk
	for (int i = 1; i <= verts_high; i++)
	{
		indices[k] = i + 1 + 33;
		indices[k + 1] = (i / 2) * 2 + 1;
		indices[k + 2] = i + 33;


		k += 3;
	}
	//create the second half of the index, with a number of triangles equal to the edge vertexes of a low resolution chunk
	for (int i = 1; i <= verts_mid; i++)
	{
		indices[k] = i * 2 + 1;
		indices[k + 1] = (i - 1) * 2 + 1;
		indices[k + 2] = i * 2 + 33;


		k += 3;
	}

	//create the index for the bottom cap
	indices[k] = 67;
	indices[k + 2] = 33;
	indices[k + 1] = 66;

	m_LOD_midToHigh = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());


	//===============================================================================
	//===============================================================================

	//===============================================================================
	//  MID TO LOW INDEX
	//===============================================================================


	indices.clear();
	indices.resize(num_faces_LM * 3);
	//std::vector<DWORD> indices2((num_faces_LM * 3));

	//create the index for the top cap
	indices[0] = 66;
	indices[1] = 67;
	indices[2] = 0;


	k = 3;
	//create the mid resolution half of the index, with a number of triangles equal to the edge vertexes of a mid resolution chunk
	for (int i = 1; i <= verts_mid; i++)
	{
		indices[k] = (i + 1) * 2 - 1;
		indices[k + 1] = (i / 2) * 4 + 34;
		indices[k + 2] = i * 2 - 1;


		k += 3;
	}
	//create the second half of the index, with a number of triangles equal to the edge vertexes of a low resolution chunk
	for (int i = 1; i <= verts_low; i++)
	{
		indices[k] = i * 4 + 34;
		indices[k + 1] = i * 4 + 34 - 4;
		indices[k + 2] = i * 4 - 1;


		k += 3;
	}

	//create the index for the bottom cap
	indices[k] = 67;
	indices[k + 2] = 33;
	indices[k + 1] = 66;

	m_LOD_midToLow = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

	//===============================================================================
	//===============================================================================



	//===============================================================================
	//  LOW TO MID INDEX
	//===============================================================================

	indices.clear();
	indices.resize(num_faces_LM * 3);
	//std::vector<DWORD> indices2((num_faces_LM * 3));

	//create the index for the top cap
	indices[0] = 66;
	indices[1] = 67;
	indices[2] = 0;


	k = 3;
	//create the mid resolution half of the index, with a number of triangles equal to the edge vertexes of a mid resolution chunk
	for (int i = 1; i <= verts_mid; i++)
	{
		indices[k] = (i + 1) * 2 + 32;
		indices[k + 1] = (i / 2) * 4 + 1;
		indices[k + 2] = i * 2 + 32;


		k += 3;
	}
	//create the second half of the index, with a number of triangles equal to the edge vertexes of a low resolution chunk
	for (int i = 1; i <= verts_low; i++)
	{
		indices[k] = i * 4 + 1;
		indices[k + 1] = i * 4 + 1 - 4;
		indices[k + 2] = i * 4 + 32;


		k += 3;
	}

	//create the index for the bottom cap
	indices[k] = 67;
	indices[k + 2] = 33;
	indices[k + 1] = 66;

	m_LOD_lowToMid = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());
	//===============================================================================
	//===============================================================================
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

	TextureManager* tm = GraphicsEngine::get()->getTextureManager();
	m_tex1 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Basecolor_A.png");
	m_displace1_1 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Height.png");
	m_norm1 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Normal.png");
	m_rough1 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Metallic.png");
	m_ambient_occ1 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_AO.png");


	m_tex2 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Basecolor.png");
	m_displace2_1 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Height.png");
	m_norm2 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Normal.png");
	m_rough2 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Metallic.png");
	m_ambient_occ2 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_AO.png");

	m_tex3 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Basecolor.png");
	m_displace3_1 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Height.png");
	m_norm3 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Normal.png");
	m_rough3 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Metallic.png");
	m_ambient_occ3 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_AO.png");

	m_tex4 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Basecolor_A.png");
	m_displace4_1 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Height.png");
	m_norm4 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Normal.png");
	m_rough4 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Metallic.png");
	m_ambient_occ4 = tm->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_AO.png");

}

void Scene13::initNoiseBuffer()
{
	m_noise.m_noise_type = Vector4D(1, 1, 1, 0);
	m_noise.m_show_rgba = Vector4D(1, 0, 0, 0);

	m_noise.m_vor_amplitude = 1.0f;
	m_noise.m_vor_gain = 0.3f;
	m_noise.m_vor_lacunarity = 2.0f;
	m_noise.m_vor_octaves = 1;
	m_noise.m_vor_cell_size = 2.0f;

	m_noise.m_per_amplitude = 0.9f;
	m_noise.m_per_gain = 0.5f;
	m_noise.m_per_lacunarity = 2.0f;
	m_noise.m_per_octaves = 2;
	m_noise.m_per_cell_size = 1.0f;

	m_noise.m_compute_cell_size = 1300;
	m_noise.m_xscale = 10;
	m_noise.m_yscale = 250;
	m_noise.m_seed = 1;

	m_noise.m_ridged_per_amplitude = 2.5f;
	m_noise.m_ridged_per_gain = 0.5f;
	m_noise.m_ridged_per_lacunarity = 2.0f;
	m_noise.m_ridged_per_octaves = 1;
	m_noise.m_ridged_per_cell_size = 2.0f;

	m_int_vor_octave = m_noise.m_vor_octaves;
	m_int_per_octave = m_noise.m_per_octaves;
	m_int_ridge_per_octave = m_noise.m_ridged_per_octaves;

}

