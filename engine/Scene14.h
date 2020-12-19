#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"
#include "PrimitiveGenerator.h"
#include "Colliders.h"
//#include "Subset.h"

class Scene14 : public Scene
{
private:
    bool m_first_time = true;

    float m_speed = 0.35f;
    float m_blend = 0;

    Vec2 m_global_light_rotation;
    float m_global_light_strength;
    Vec3 m_light_color;
    Vec3 m_ambient_light_color;



    bool m_show_hitboxes = false;
    //temporary
    bool hit = false;

    Collider* obj1 = nullptr;
    Collider* obj2 = nullptr;
    Vec3 pos1 = {};
    Vec3 prevpos1 = {};
    Vec3 pos2 = {};
    Vec3 markerpos = {};
    Vec3 marker2pos = {};
    PrimitivePtr marker = nullptr;
    float mass = 1.0f;
    Vec3 movement = Vec3(0.04, 0, 0.04);

    Vec3 rot1;
    Vec3 rot2;
    float r1, r2;
    float len1, len2;
    bool show_bbs = false;

    Vec3 start1, start2;

    bool is_simulate = false;

public:
    Scene14(SceneManager*);
    ~Scene14();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

    //virtual void outputSceneData(std::string filename);
    //virtual void loadSceneData(std::string filename);

    void renderCollider(Vec3 pos, Collider* c, Vec3 col, int type, float r, float len);

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};