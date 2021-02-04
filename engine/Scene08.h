#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "Texture3D.h"
#include "ConstantBufferFormats.h"

class Scene08 : public Scene
{
private:

    SkinnedMeshPtr m_light;
    SkinnedMeshPtr m_model;
    bool m_first_time = true;

    Texture3DPtr m_tex3D_main;
    Texture3DPtr m_tex3D_detail;
    TexturePtr m_blue_noise;
    bool m_show_vertices = false;

    float m_speed = 3.0f;

    Vec2 m_global_light_rotation;
    float m_global_light_strength;
    Vec3 m_light_color;
    Vec3 m_ambient_light_color;

    cb_cloud m_cloud_props;
    cb_cloud m_default;
    cb_cloud m_preset1;
    cb_cloud m_preset2;
    cb_cloud m_preset3;
    cb_cloud m_preset4;

public:
    Scene08(SceneManager*);
    ~Scene08();

    virtual void update(float delta) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};