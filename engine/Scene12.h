#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"
#include "PrimitiveGenerator.h"
//#include "Subset.h"

class Scene12 : public Scene
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

    cb_noise m_noise;
    cb_cloud m_cloud_props;
    Texture3DPtr m_tex3D;

    bool m_show_hitboxes = false;
    //temporary
    bool hit = false;

public:
    Scene12(SceneManager*);
    ~Scene12();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

    //virtual void outputSceneData(std::string filename);
    //virtual void loadSceneData(std::string filename);

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};