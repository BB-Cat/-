#pragma once
#include <stack>
#include "Prerequisites.h"
#include "Scene.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

class SceneManager
{
private:
    std::stack<std::unique_ptr<Scene>> m_scene_stack;
    int m_next_scene;
    bool m_clear_flag;

public:
    SceneManager();
    ~SceneManager();


    void execute(const float& delta, const float& width, const float& height);

    enum SCENE
    {
        SCENESELECT,
        SCENE01,
        SCENE02,
        SCENE03,
        SCENE04,
        SCENE05,
        SCENE06,
        SCENE07,
        SCENE08,
        SCENE09,
    };

    void changeScene(const SCENE, bool clear_current_scene = false);
};
