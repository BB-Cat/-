#pragma once
#include <vector>
#include "Prerequisites.h"
#include "ConstantBufferFormats.h"
#include "Colliders.h"
#include "Vector3D.h"
#include "Subset.h"


//class which holds prefab data for game scenes.  this is just an early prototype so it needs lots of work still.
struct PrefabMesh
{
	//this should be a static mesh.  we need to revise this in the future.
	SkinnedMeshPtr mesh;
	Collider* collider = nullptr;

	Vector3D default_scale = Vector3D(1, 1, 1);
	Vector3D default_rot =   Vector3D(0, 3.1415f, 0);
	int default_shader = Shaders::LAMBERT_RIMLIGHT;
};

class PrefabManager
{
private:
	PrefabManager();
	static PrefabManager* instance;

public:
	static PrefabManager* get()
	{
		if (instance == nullptr) instance = new PrefabManager();
		return instance;
	}
	~PrefabManager();

	bool createPrefab(std::wstring file, std::string name);
	std::vector<PrefabMesh> getPrefabs() { return m_prefabs; }
	std::vector<std::string> getPrefabNames() { return m_prefab_names; }
	std::vector<std::wstring> getPrefabFiles() { return m_prefab_files; }

	PrefabMesh* findPrefab(std::string name);

	bool ImGuiModifyPrefabs();

	void renderEditingPrefab();

private:
	void renderEditingBoundingBox();

private:
	std::vector<PrefabMesh> m_prefabs;
	std::vector<std::string> m_prefab_names;
	std::vector<std::wstring> m_prefab_files;

private: //ModifyPrefabségópÇÃïœêî
	int m_focused_prefab = -1;
	char m_name[128] = {};

	Vector3D m_focused_scale;
	Material_Obj m_focused_material;
	bool m_show_collider = false;
};