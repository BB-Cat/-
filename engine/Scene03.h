#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"

class Scene03 : public Scene
{
private:
    //Terrain* m_terrain;
    //Terrain* m_terrain2;
    //Terrain* m_terrain3;
    //Terrain* m_terrain4;
    //Terrain* m_terrain5;
    //Terrain* m_terrain6;

    std::shared_ptr<TerrainManager> m_terrain;

    int m_toggle_HD = 2;
    bool m_rast = false;

public:
    Scene03(SceneManager*);
    ~Scene03();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;
    //virtual void render(float delta) override;
private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};