#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "Sprite.h"
#include "ParticleSystem.h"

class Scene01 : public Scene
{
private:
    SkinnedMeshPtr m_mesh;
    SkinnedMeshPtr m_sky;

    TexturePtr m_diffuse;
    TexturePtr m_normal;
    TexturePtr m_roughness;
    TexturePtr m_ao;

    Vec3 m_light_pos;

    bool m_first_time = true;

    float m_tesselation;
    float m_seconds;

private: //ImGui variables
    bool m_show_sky;
    int m_shader_type;
    Vec2 m_global_light_rotation;
    float m_global_light_strength;
    Vec3 m_light_color;
    Vec3 m_ambient_light_color;

public:
    Scene01(SceneManager*);
    ~Scene01();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;
private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
    //virtual void render(float delta) override;
};