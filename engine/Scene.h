#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"

class SceneManager;

class Scene
{
protected:
    SceneManager* p_manager;
    int m_timer;
    int m_state;

    Vector3D m_scene_light_dir;

public:
    Scene(SceneManager* manager) :m_timer(0), m_state(0), m_scene_light_dir(Vector3D(0.3f, -0.4f, 0.3f)), p_manager(manager) {};
    virtual ~Scene() {};

    virtual void update(float delta, const float& width, const float& height) = 0;
    virtual void render(float delta, const float& width, const float& height);
    virtual void imGuiRender() = 0;
private:
    //shader pass for shadow mapping
    virtual void shadowRenderPass(float delta) = 0;
    //main shader pass for rendering
    virtual void mainRenderPass(float delta) = 0;
    //post effect passes (optional)
    virtual void addRenderPass(float delta) {}
};
