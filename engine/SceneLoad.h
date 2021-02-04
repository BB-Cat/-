#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "Sprite.h"
#include "ConstantBufferFormats.h"
#include <thread>

class SceneLoad : public Scene
{
private:
    SpritePtr m_loading_screen;
    SpritePtr m_text;

    cb_time m_time;

    bool m_first_time = true;
    float m_seconds;

    std::unique_ptr<Scene> m_target_scene = nullptr;
    std::shared_ptr<std::thread> m_load_thread = nullptr;

    bool m_is_init = false;

private: //ImGui variables
    bool m_show_sky;
    int m_shader_type;
    Vec2 m_global_light_rotation;
    float m_global_light_strength;
    Vec3 m_light_color;
    Vec3 m_ambient_light_color;

public:
    SceneLoad(SceneManager*, int nextscene);
    ~SceneLoad();

    virtual void update(float delta) override;
    virtual void imGuiRender() override;
private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;

    void threadLoadNextScene(int scene_id);
    void swapScenes();

    //virtual void render(float delta) override;
};