#pragma once
#include "Mesh.h"
#include <vector>

#include "Subset.h"

//static mesh class which pulls data from .OBJ files
class StaticMesh: public Mesh
{
public:
	StaticMesh(const wchar_t* full_path);
	StaticMesh(const wchar_t* full_path, bool is_flipped);
	~StaticMesh();

	virtual void renderMesh(float elapsed_time, Vector3D scale, Vector3D position, Vector3D rotation, int shader, bool is_textured, float animation_speed = 1.0f) override;

private:
	//creates a full filepath for a related file such as an MTL file from another file's filepath in the same folder
	std::wstring createFilepath(std::wstring orig_path, UINT size_orig, const wchar_t* target_file, UINT size_target);
	void loadMtlFile(std::wstring full_path);
private:
	std::vector<Subset_Obj> m_subs;
	std::vector<Material_Obj> m_mats;
};