#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"


class Scene07 : public Scene
{
private:

    SkinnedMeshPtr m_sky;
    SkinnedMeshPtr m_model;
    Texture3DPtr m_tex;
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

    float m_move = 0;
    int m_is_move = 0;

    cb_noise m_noise;
    float m_seed = 1.0f;
public:
    Scene07(SceneManager*);
    ~Scene07();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;

    void load3DTexture();
};