#pragma once
#include "ConstantBufferFormats.h"
#include "PrimitiveGenerator.h"
#include "Subset.h"
#include "Vector3D.h"
#include "Colliders.h"


//temp 
#include "GraphicsEngine.h"
#include "ResourceManager.h"   

//the world object manager will store a vector of loaded meshes.  if a create mesh call is made and the mesh doesnt exist yet,
//it is created and an object holding pointers to its vertex and index buffers will be returned.
//for every collision detection function, the default shape vector will be confirmed, 
//then the collision will be run again for mesh pointers holding that type of collider

class WorldObjectManager
{
private:
	static WorldObjectManager* instance;
    WorldObjectManager();

public:
	static WorldObjectManager* get();
	~WorldObjectManager() 
    {
        m_objects.clear();
    }

    static void release()
    {
        if (instance == nullptr) return;

        delete instance; 
        instance = nullptr;
    }

	void outputSceneData(std::string filename);
	bool loadSceneData(std::string filename);
    void clear();

	void imGuiRender();
	void render(float elapsed_time, bool skip_highlight = false);
    //function to render the currently selected object with a highlight
    void renderSelectedHighlight();
    //function to render bounding boxes for world objects that are toggled to show bounding boxes
    void renderBoundingBoxes(bool force_show = false);
    

    //this is all just a prototype and will be reworked drastically for improved hit collisions in the future
    Vec3 BBoxCollisionResolveDiscrete(Vec3 old_pos, Vec3 new_pos, Vec3 size);

    //continuous bounding box collision resolution function
    Vec3 BBoxCollisionResolveCont(Vec3 old_pos, Vec3 new_pos, Vec3 size);
    //continuous bounding box collision check
    bool BBoxCollisionCheckCont(Vec3 old_pos, Vec3 new_pos, Vec3 size);

    bool getShowPrefabEditor() { return m_show_prefab_editor; }
private:
	std::vector<WorldObjectPtr> m_objects;
    //a 2D vector with a seperate vector for each type of collider

    bool m_show_obj_window = false;
    bool m_show_mat_window = false;
    bool m_show_shader_window = false;
    bool m_show_texture_window = false;

    bool m_show_prefab_editor = false;
    char m_texname[128] = {};
    char m_meshname[128] = {};
    char m_scenename[128] = {};
    char m_loadscene[128] = {};
    int m_object_id = 0;
    Vec3 m_focused_pos;
    Vec3 m_focused_scale;
    Vec3 m_focused_rot;
    Material_Obj m_focused_material;

    std::string current_file = {};
};

