#pragma once
#include "ConstantBufferFormats.h"
#include "PrimitiveGenerator.h"
#include "Subset.h"
#include "Vector3D.h"
#include "Subset.h"

class WorldObjectManager
{
private:
	static WorldObjectManager* instance;
	WorldObjectManager() {}
public:
	static WorldObjectManager* get();
	~WorldObjectManager() {}

	void outputSceneData(std::string filename);
	void loadSceneData(std::string filename);
    void clear();

	void imGuiRender();
	void render();

    Vector3D CubeAABBCollision(Vector3D old_pos, Vector3D new_pos, Vector3D size);

private:
	std::vector<CubePtr> m_cubes;

    bool m_show_obj_window = false;
    bool m_show_mat_window = false;
    bool m_show_shader_window = false;
    bool m_show_texture_window = false;
    char m_texname[128] = {};
    char m_scenename[128] = {};
    char m_loadscene[128] = {};
    int m_cube_ID = 0;
    Vector3D m_spawn_pos;
    Vector3D m_spawn_scale;
    Vector3D m_spawn_rot;

    Material_Obj m_spawn_mat;
};

