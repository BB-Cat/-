#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"
#include "PrimitiveGenerator.h"
#include "Colliders.h"
#include "FbxAnm.h"
//#include "Subset.h"

class Scene15 : public Scene
{
private:
    bool m_first_time = true;
    bool m_is_play = false;
    float m_play_speed = 1.5f;

    int m_highlighted_frame = 0;
    int m_highlighted_hitbox = -1;

    float m_speed = 0.35f;
    float m_blend = 0;

    Vec2 m_global_light_rotation;
    float m_global_light_strength;
    Vec3 m_light_color;
    Vec3 m_ambient_light_color;

    SkinnedMeshPtr m_ground;
    PrimitivePtr marker = nullptr;
    SkinnedMeshPtr m_model;

	Fbx_Anm* m_anm = nullptr;
    Vec3 m_modelpos = {};
    std::vector<std::shared_ptr<AnmHitbox>> hb_vec;

    bool m_show_hitboxes = false;
    //temporary
    bool hit = false;

public:
    Scene15(SceneManager*);
    ~Scene15();

    virtual void update(float delta) override;
    virtual void imGuiRender() override;

    //virtual void outputSceneData(std::string filename);
    //virtual void loadSceneData(std::string filename);

    void renderCapsule(Collider* capsule);

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};