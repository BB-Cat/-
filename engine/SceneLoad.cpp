#include "SceneLoad.h"
#include "AppWindow.h"
#include "SkinnedMesh.h"
#include "CameraManager.h"
#include "GraphicsEngine.h"
#include "ConstantBufferSystem.h"
#include "Lighting.h"
#include "Terrain.h"
#include "VectorToArray.h"
#include "DeviceContext.h"

#include "Scene01.h"
#include "Scene02.h"
#include "Scene03.h"
#include "Scene04.h"
#include "Scene05.h"
#include "Scene06.h"
#include "Scene07.h"
#include "Scene08.h"
#include "Scene09.h"
#include "Scene10.h"
#include "Scene11.h"
#include "Scene12.h"
#include "Scene13.h"
#include "Scene14.h"
#include "Scene15.h"
#include "Scene16.h"
#include "Scene17.h"




SceneLoad::SceneLoad(SceneManager* sm, int nextscene) : Scene(sm)
{
	AppWindow::toggleDeferredPipeline(false);
	CameraManager::get()->setCamState(FREE);
	CameraManager::get()->setCamPos(Vec3(0, 0, 0));
	CameraManager::get()->setCamRot(Vec2(0, 0));

	m_loading_screen = std::make_shared<Sprite>(Shaders::SCREENSPACE_LOADSCREEN);
    //m_loading_screen->setTexture(GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\BGTex\\cyber.jpg"));
	m_text = GraphicsEngine::get()->getSpriteManager()->createSpriteFromFile(L"..\\Assets\\Load Icon\\loading.png");

	//std::shared_ptr<std::thread> t(new std::thread(&TerrainManager::threadLoadChunkTxt, this, pos, i));
    std::shared_ptr<std::thread> t(new std::thread(&SceneLoad::threadLoadNextScene, this, nextscene));
    m_load_thread = t;
}

SceneLoad::~SceneLoad()
{
    if (m_load_thread != nullptr) m_load_thread->detach();
}

void SceneLoad::update(float delta)
{
	CameraManager::get()->update(delta);

	m_time.m_elapsed = delta;
	m_time.m_time += delta;
	GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTimeBuffer(m_time);

    if (m_is_init) swapScenes();
}

void SceneLoad::imGuiRender()
{
	//ImGui::SetNextWindowSize(ImVec2(215, 45));
	//ImGui::SetNextWindowPos(ImVec2(0, 20));
	//ImGui::SetNextWindowBgAlpha(0.6f);
	//ImGui::Begin("Return", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	//if (ImGui::Button("Main Menu", ImVec2(200, 30))) p_manager->changeScene(SceneManager::SCENESELECT, false);
	//ImGui::End();

}

void SceneLoad::shadowRenderPass(float delta)
{
}

void SceneLoad::mainRenderPass(float delta)
{
	m_loading_screen->renderScreenSpaceShader();
	float sin1 = sinf(m_time.m_time);
	m_text->renderScreenSpaceSprite(Vec2(0.4, 0.4), Vec2(0.575, 0.075 + 0.01f * sin1), 0, Vec2(0, 0), Vec2(400, 400), Vec2(0, 200), 0.8f);
}

void SceneLoad::threadLoadNextScene(int scene_id)
{
	//make a new scene pointer for the scene type we are supposed to load
	//m_target_scene

    switch (scene_id)
    {
    case SceneManager::SCENE01:
        m_target_scene = std::make_unique<Scene01>(p_manager);
        break;
    case SceneManager::SCENE02:
        m_target_scene = std::make_unique<Scene02>(p_manager);
        break;
    case SceneManager::SCENE03:
        m_target_scene = std::make_unique<Scene03>(p_manager);
        break;
    case SceneManager::SCENE04:
        m_target_scene = std::make_unique<Scene04>(p_manager);
        break;
    case SceneManager::SCENE05:
        m_target_scene = std::make_unique<Scene05>(p_manager);
        break;
    case SceneManager::SCENE06:
        m_target_scene = std::make_unique<Scene06>(p_manager);
        break;
    case SceneManager::SCENE07:
        m_target_scene = std::make_unique<Scene07>(p_manager);
        break;
    case SceneManager::SCENE08:
        m_target_scene = std::make_unique<Scene08>(p_manager);
        break;
    case SceneManager::SCENE09:
        m_target_scene = std::make_unique<Scene09>(p_manager);
        break;
    case SceneManager::SCENE10:
        m_target_scene = std::make_unique<Scene10>(p_manager);
        break;
    case SceneManager::SCENE11:
        m_target_scene = std::make_unique<Scene11>(p_manager);
        break;
    case SceneManager::SCENE12:
        m_target_scene = std::make_unique<Scene12>(p_manager);
        break;
    case SceneManager::SCENE13:
        m_target_scene = std::make_unique<Scene13>(p_manager);
        break;
    case SceneManager::SCENE14:
        m_target_scene = std::make_unique<Scene14>(p_manager);
        break;
    case SceneManager::SCENE15:
        m_target_scene = std::make_unique<Scene15>(p_manager);
        break;
    case SceneManager::SCENE16:
        m_target_scene = std::make_unique<Scene16>(p_manager);
        break;
    case SceneManager::SCENE17:
        m_target_scene = std::make_unique<Scene17>(p_manager);
        break;
    }

	//once the scene is initialized, tell the load scene it can swap out at the scene manager
    m_is_init = true;
}

void SceneLoad::swapScenes()
{
    m_load_thread->join();
    p_manager->endLoad(m_target_scene);
}
