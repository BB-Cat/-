#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"


class Scene09 : public Scene
{
private:

    SkinnedMeshPtr m_sky;
    SkinnedMeshPtr m_model;
    Texture3DPtr m_tex3D_main;
    Texture3DPtr m_tex3D_detail;
    TexturePtr m_blue_noise;

    bool m_first_time = true;

    float m_speed = 0.35f;

    Vec2 m_global_light_rotation;
    float m_global_light_strength;
    Vec3 m_light_color;
    Vec3 m_ambient_light_color;


    int m_int_vor_octave;
    int m_int_vor_frequency;
    int m_int_per_octave;
    int m_int_per_frequency;

    cb_noise m_noise;
    cb_cloud m_cloud_props;
    Texture3DPtr m_tex3D;
    bool m_show_tex3D = false;

    float m_seed = 1.0f;
public:
    Scene09(SceneManager*);
    ~Scene09();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;

};