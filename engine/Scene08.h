#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "Texture3D.h"
#include "ConstantBufferFormats.h"

class Scene08 : public Scene
{
private:

    SkinnedMeshPtr m_sky;
    SkinnedMeshPtr m_model;
    bool m_first_time = true;

    Texture3DPtr m_tex3D_main;
    Texture3DPtr m_tex3D_detail;
    TexturePtr m_blue_noise;
    bool m_show_tex3D = false;

    float m_speed = 0.35f;

    Vector2D m_global_light_rotation;
    float m_global_light_strength;
    Vector3D m_light_color;
    Vector3D m_ambient_light_color;

    cb_cloud m_cloud_props;

public:
    Scene08(SceneManager*);
    ~Scene08();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};