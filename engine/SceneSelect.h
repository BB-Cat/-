#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "Sprite.h"
#include "ParticleSystem.h"

class SceneSelect : public Scene
{
private:
    static bool m_popup_toggle;
    TexturePtr m_tex1;
    //TexturePtr m_compute_texture;

    void* compute_image_data;
    ComputeShaderPtr m_cs;
    ID3D11ShaderResourceView* m_srv;
private: //ImGui variables

public:
    SceneSelect(SceneManager*);
    ~SceneSelect();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

    void makeComputeShaderTextureTest();
private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
    //virtual void render(float delta) override;
};