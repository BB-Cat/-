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


private: //ImGui variables

public:
    SceneSelect(SceneManager*);
    ~SceneSelect();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
    //virtual void render(float delta) override;
};