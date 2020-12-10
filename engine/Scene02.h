#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "Player.h"
#include "CapsuleHB.h"

class Scene02 : public Scene
{
private:
	Terrain* m_terrain;
    bool m_first_time = true;

    SkinnedMeshPtr m_mesh1;
    SkinnedMeshPtr m_mesh2;
    SkinnedMeshPtr m_mesh3;
    SkinnedMeshPtr m_mesh4;

    SkinnedMeshPtr m_ground;
    SkinnedMeshPtr m_sky;

private: //ImGui variables
    Vec2 m_global_light_rotation;


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