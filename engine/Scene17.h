#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "Texture3D.h"
#include "ConstantBufferFormats.h"
#include "Clouds.h"

class Scene17 : public Scene
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

    cloud_struct m_clouds;
    int m_spherecount = 15;
    cb_compute_raymarch m_raymarch_buffer;

public:
    Scene17(SceneManager*);
    ~Scene17();

    virtual void update(float delta) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;

    void generateCloud();
};