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
    void imGui();

    enum SCENE
    {
        SCENESELECT,
        SCENE01,  //shaders (several need reworking)
        SCENE02,  //shadow mapping
        SCENE03,  //terrain generator (needs to be multi threaded and streamlined)
        SCENE04,  //dynamic terrain
        SCENE05,  //tesselation (needs reworking)
        SCENE06,  //character animation
        SCENE07,  //noise generator (voronoi and perlin)
        SCENE08,  //volumetric cloud cube
        SCENE09,  //weather map clouds
        SCENE10,  //character movement
        SCENE11,  //stage creation
        SCENE12,  //game style scene select
        SCENE13,  //compute shader practice - ray tracing
    };

    void changeScene(const SCENE, bool clear_current_scene = false);
};
