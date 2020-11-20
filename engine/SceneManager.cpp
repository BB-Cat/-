#include "SceneManager.h"
#include "SceneSelect.h"
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

SceneManager::SceneManager() :m_next_scene(0), m_clear_flag()
{
    m_scene_stack.emplace(std::make_unique<SceneSelect>(this));
}

SceneManager::~SceneManager()
{
    while (!m_scene_stack.empty())
    {
        m_scene_stack.pop();
    }
}

void SceneManager::execute(const float& delta, const float& width, const float& height)
{
    
    if (m_clear_flag == true) m_clear_flag = false;

    m_scene_stack.top()->update(delta, width, height);
    m_scene_stack.top()->render(delta, width, height);
    
}

void SceneManager::imGui()
{
    //render whatever imgui dialogue the current scene needs
    m_scene_stack.top()->imGuiRender();
}


void SceneManager::changeScene(const SCENE next_scene, bool clear_current_scene)
{
    if (clear_current_scene == true)
    {
        m_clear_flag = true;
        m_scene_stack.pop();
        m_next_scene = next_scene;

        switch (next_scene)
        {
        case SCENESELECT:
            m_scene_stack.emplace(std::make_unique<SceneSelect>(this));
            break;
        case SCENE01:
            m_scene_stack.emplace(std::make_unique<Scene01>(this));
            break;
        case SCENE02:
            m_scene_stack.emplace(std::make_unique<Scene02>(this));
            break;
        case SCENE03:
            m_scene_stack.emplace(std::make_unique<Scene03>(this));
            break;
        case SCENE04:
            m_scene_stack.emplace(std::make_unique<Scene04>(this));
            break;
        case SCENE05:
            m_scene_stack.emplace(std::make_unique<Scene05>(this));
            break;
        case SCENE06:
            m_scene_stack.emplace(std::make_unique<Scene06>(this));
            break;
        case SCENE07:
            m_scene_stack.emplace(std::make_unique<Scene07>(this));
            break;
        case SCENE08:
            m_scene_stack.emplace(std::make_unique<Scene08>(this));
            break;
        case SCENE09:
            m_scene_stack.emplace(std::make_unique<Scene09>(this));
            break;
        case SCENE10:
            m_scene_stack.emplace(std::make_unique<Scene10>(this));
            break;
        case SCENE11:
            m_scene_stack.emplace(std::make_unique<Scene11>(this));
            break;
        case SCENE12:
            m_scene_stack.emplace(std::make_unique<Scene12>(this));
            break;
        case SCENE13:
            m_scene_stack.emplace(std::make_unique<Scene13>(this));
            break;
        }
    }
    else
    {
        switch (next_scene)
        {
        case SCENESELECT:
            m_scene_stack.emplace(std::make_unique<SceneSelect>(this));
            break;
        case SCENE01:
            m_scene_stack.emplace(std::make_unique<Scene01>(this));
            break;
        case SCENE02:
            m_scene_stack.emplace(std::make_unique<Scene02>(this));
            break;
        case SCENE03:
            m_scene_stack.emplace(std::make_unique<Scene03>(this));
            break;
        case SCENE04:
            m_scene_stack.emplace(std::make_unique<Scene04>(this));
            break;
        case SCENE05:
            m_scene_stack.emplace(std::make_unique<Scene05>(this));
            break;
        case SCENE06:
            m_scene_stack.emplace(std::make_unique<Scene06>(this));
            break;
        case SCENE07:
            m_scene_stack.emplace(std::make_unique<Scene07>(this));
            break;
        case SCENE08:
            m_scene_stack.emplace(std::make_unique<Scene08>(this));
            break;
        case SCENE09:
            m_scene_stack.emplace(std::make_unique<Scene09>(this));
            break;
        case SCENE10:
            m_scene_stack.emplace(std::make_unique<Scene10>(this));
            break;
        case SCENE11:
            m_scene_stack.emplace(std::make_unique<Scene11>(this));
            break;
        case SCENE12:
            m_scene_stack.emplace(std::make_unique<Scene12>(this));
            break;
        case SCENE13:
            m_scene_stack.emplace(std::make_unique<Scene13>(this));
            break;
        }
    }
}
