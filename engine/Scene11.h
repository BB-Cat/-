#pragma once
#include "SceneManager.h"
#include "Prerequisites.h"
#include "TextRenderer.h"
#include "ConstantBufferFormats.h"
#include "PrimitiveGenerator.h"
//#include "Subset.h"

class Scene11 : public Scene
{
private:
    //bool m_show_obj_window = false;
    //bool m_show_mat_window = false;
    //bool m_show_shader_window = false;
    //bool m_show_texture_window = false;
    //char m_texname[128] = {};
    //char m_scenename[128] = {};
    //char m_loadscene[128] = {};
    //int m_cube_ID = 0;
    //Vector3D m_spawn_pos;
    //Vector3D m_spawn_scale;
    //Vector3D m_spawn_rot;

    //Material_Obj m_spawn_mat;

    SkinnedMeshPtr m_sky;
    SkinnedMeshPtr m_model;
    SkinnedMeshPtr m_floor;

    bool m_first_time = true;
    //std::shared_ptr<Cube> m_cube;
    //std::vector<CubePtr> m_cubes;


    float m_speed = 0.35f;
    float m_blend = 0;

    Vector2D m_global_light_rotation;
    float m_global_light_strength;
    Vector3D m_light_color;
    Vector3D m_ambient_light_color;

    cb_noise m_noise;
    cb_cloud m_cloud_props;
    Texture3DPtr m_tex3D;


public:
    Scene11(SceneManager*);
    ~Scene11();

    virtual void update(float delta, const float& width, const float& height) override;
    virtual void imGuiRender() override;

    //virtual void outputSceneData(std::string filename);
    //virtual void loadSceneData(std::string filename);

private:
    virtual void shadowRenderPass(float delta) override;
    virtual void mainRenderPass(float delta) override;
};