#pragma once
#include "ConstantBufferFormats.h"
#include "PrimitiveGenerator.h"
#include "Subset.h"
#include "Vector3D.h"
#include "Subset.h"
#include "Colliders.h"

//the world object manager will store a vector of loaded meshes.  if a create mesh call is made and the mesh doesnt exist yet,
//it is created and an object holding pointers to its vertex and index buffers will be returned.
//for every collision detection function, the default shape vector will be confirmed, 
//then the collision will be run again for mesh pointers holding that type of collider

//class MeshObject
//{
//public:
//    MeshObject(SkinnedMeshPtr mesh, Collider* collider) : m_mesh(mesh), m_collider(collider) {}
//
//private:
//    SkinnedMeshPtr m_mesh;
//    Collider* m_collider;
//
//    Vector3D m_scale, m_pos, m_rotation;
//};

class WorldObjectManager
{
private:
	static WorldObjectManager* instance;
	WorldObjectManager() 
    {
        m_meshes.resize(ColliderTypes::MAX);
    }
public:
	static WorldObjectManager* get();
	~WorldObjectManager() {}

	void outputSceneData(std::string filename);
	void loadSceneData(std::string filename);
    void clear();

	void imGuiRender();
	void render();
    //function to render the currently selected object with a highlight
    void renderSelectedHighlight();

    //this is all just a prototype and will be reworked drastically for improved hit collisions in the future
    Vector3D CubeAABBCollision(Vector3D old_pos, Vector3D new_pos, Vector3D size);

private:
	std::vector<WorldObjectPtr> m_cubes;
    //a 2D vector with a seperate vector for each type of collider
    std::vector<std::vector<WorldObjectPtr>> m_meshes;

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

