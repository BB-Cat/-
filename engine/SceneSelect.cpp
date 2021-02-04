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
#include "Sprite.h"


bool SceneSelect::m_popup_toggle = true;

SceneSelect::SceneSelect(SceneManager* sm) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);

	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vec3(8, 7, -6));

	//m_tex1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\bg.jpg");
	m_background = std::make_shared<Sprite>(Shaders::SCREENSPACE_SELECTSCREEN);
	m_background->setTexture(GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\BGTex\\cyber.jpg"));

}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::update(float delta)
{
	m_time.m_elapsed = delta;
	m_time.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTimeBuffer(m_time);
}

void SceneSelect::imGuiRender()
{
	//ImGui::SetNextWindowSize(ImVec2(1024, 720));
	//ImGui::SetNextWindowPos(ImVec2(-20, 10));
	//ImGui::SetNextWindowBgAlpha(0.0f);
	//ImGui::Begin("Background", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs);
	//ImTextureID t = m_tex1->getSRV();
	//ImGui::Image(t, ImVec2(1024, 720));

	//ImGui::End();

	//=====================================================
	//  Create the scene interface window
	//-----------------------------------------------------
	//ImGui::SetNextWindowSize(ImVec2(1000, 700));
	Vec2 screen = AppWindow::getScreenSize();
	ImGui::SetNextWindowPos(ImVec2(screen.x / 2, screen.y / 2), 0, ImVec2(0.5, 0.5));
	ImGui::SetNextWindowBgAlpha(0.75f);
	////create the test window
	ImGui::Begin("Scene Selection", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);

	ImVec2 size = ImVec2(280.0f, 30.0f);
	ImGui::Text("ESC: Show/Hide Mouse");

	int choice = -1;

	if (ImGui::Button("Shaders",			size)) choice = SceneManager::SCENE01;
	if (ImGui::Button("ShadowMapping",		size)) choice = SceneManager::SCENE02;
	//if (ImGui::Button("Terrain Generator",  size)) choice = SceneManager::SCENE03;
	if (ImGui::Button("Text Based Terrain",	size)) choice = SceneManager::SCENE04;
	if (ImGui::Button("Compute Shader Terrain", size)) choice = SceneManager::SCENE13;
	//if (ImGui::Button("Tesselation",		  size)) choice = SceneManager::SCENE05;
	//if (ImGui::Button("Character Animation",size)) choice = SceneManager::SCENE06;
	if (ImGui::Button("Noise",				size)) choice = SceneManager::SCENE07;
	if (ImGui::Button("Volumetric Clouds",	size)) choice = SceneManager::SCENE08;
	if (ImGui::Button("Ray Tracing", size)) choice = SceneManager::SCENE16;
	if (ImGui::Button("SphereClouds", size)) choice = SceneManager::SCENE17;
	//if (ImGui::Button("Weather Map",		size)) choice = SceneManager::SCENE09;
	//if (ImGui::Button("Character Movement", size)) choice = SceneManager::SCENE10;
	if (ImGui::Button("Stage Creator",		size)) choice = SceneManager::SCENE11;
	if (ImGui::Button("Stage Example",		size)) choice = SceneManager::SCENE12;
	//if (ImGui::Button("Collision Test",     size)) choice = SceneManager::SCENE14;
	if (ImGui::Button("Hitbox Modifier",    size)) choice = SceneManager::SCENE15;

	//if (ImGui::Button("Load",				size)) choice = SceneManager::SCENELOAD;

	if (choice > -1)
	{
		p_manager->changeScene(choice, true);
	}

	ImGui::End();

	//ImGui::SetNextWindowSize(ImVec2(300, 50));
	ImGui::SetNextWindowPos(ImVec2(700, 650));
	ImGui::SetNextWindowBgAlpha(0.3f);
	////create the test window
	ImGui::Begin("NoTitle", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.2f, 0.7f), "Email : carson.hage@outlook.com");
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Phone : 070-4292-5277");
	ImGui::End();
}

void SceneSelect::shadowRenderPass(float delta)
{
}

void SceneSelect::mainRenderPass(float delta)
{
	m_background->renderScreenSpaceShader(true);
}