#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"

//temp
#include "PrimitiveGenerator.h"

class Scene11 : public Scene
{
private:
    bool m_show_window = false;
    Vector3D m_spawn_pos;
    Vector3D m_spawn_scale;
    Vector3D m_spawn_rot;

    SkinnedMeshPtr m_sky;
    SkinnedMeshPtr m_model;
    SkinnedMeshPtr m_floor;
    //std::shared_ptr<Cube> m_cube;
    std::vector<CubePtr> m_cubes;


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
    Scene11(SceneManager*);
    ~Scene11();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};