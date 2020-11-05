#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "Player.h"
#include "CapsuleHB.h"

class Scene02 : public Scene
{
private:
	Terrain* m_terrain;
    bool m_is_first_frame;

    SkinnedMeshPtr m_mesh1;
    SkinnedMeshPtr m_mesh2;
    SkinnedMeshPtr m_mesh3;

    SkinnedMeshPtr m_ground;

private: //ImGui variables
    Vector2D m_global_light_rotation;


public:
    Scene02(SceneManager*);
    ~Scene02();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;
   // virtual void render(float delta) override;
private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};