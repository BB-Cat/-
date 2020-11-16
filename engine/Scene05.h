#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"

class Scene05 : public Scene
{
private:

    std::shared_ptr<TerrainManager> m_terrain;
    //TerrainPtr m_terrain;
    SkinnedMeshPtr m_sky;

    bool m_first_time = true;

    int m_toggle_HD = 2;
    float m_speed = 0.35f;
    float m_bump_height = 0.001f;
    bool m_rast = false;

    bool m_toggle_norm = false;

    Vector2D m_global_light_rotation;
    float m_global_light_strength;
    Vector3D m_light_color;
    Vector3D m_ambient_light_color;

    int m_max_tess = 8;
    int m_min_tess = 1;
    int m_min_tess_range = 200;
    int m_max_tess_range = 120;

public:
    Scene05(SceneManager*);
    ~Scene05();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};