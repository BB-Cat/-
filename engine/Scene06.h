#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"

class Scene06 : public Scene
{
private:

    SkinnedMeshPtr m_sky;
    SkinnedMeshPtr m_model;

    bool m_first_time = true;

    float m_speed = 0.35f;
    float m_blend = 0;

    Vec2 m_global_light_rotation;
    float m_global_light_strength;
    Vec3 m_light_color;
    Vec3 m_ambient_light_color;


public:
    Scene06(SceneManager*);
    ~Scene06();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};