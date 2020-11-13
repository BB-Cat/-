#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"

class Scene10 : public Scene
{
private:

    SkinnedMeshPtr m_sky;
    SkinnedMeshPtr m_model;
    SkinnedMeshPtr m_floor;

    float m_speed = 0.35f;
    float m_blend = 0;

    Vector2D m_global_light_rotation;
    float m_global_light_strength;
    Vector3D m_light_color;
    Vector3D m_ambient_light_color;

    cb_noise m_noise;
    cb_cloud m_cloud_props;
    Texture3DPtr m_tex3D;


public:
    Scene10(SceneManager*);
    ~Scene10();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};