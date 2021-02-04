#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "ConstantBufferFormats.h"

class Scene16 : public Scene
{
private:
    bool m_first_time = true;

    float m_timer = 0;
    Vec2 m_global_light_rotation;
    float m_global_light_strength;
    Vec3 m_light_color;
    Vec3 m_ambient_light_color;

    SpritePtr m_sprite;
    TexturePtr sky;
    //ComputeShaderPtr m_compute_raymarch;
    TextureComputeShaderPtr m_compute_raymarch;


    int m_tracecount = 1;
    int m_spherecount = 15;
    bool m_stop_update = false;
    cb_compute_raymarch m_raymarch_buffer;

public:
    Scene16(SceneManager*);
    ~Scene16();

    virtual void update(float delta) override;
    virtual void imGuiRender() override;



private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;

private:
    void initSpriteTexture();
    void makeComputeShaderTexture();

    void generateSpheres();
    void updateSphereData();
};