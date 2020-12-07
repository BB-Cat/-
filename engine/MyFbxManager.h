#pragma once
#include "Prerequisites.h"
#include "VertexMesh.h"
#include <vector>
#include <map>
#include "Subset.h"
#include "Skeleton.h"

#include <fbxsdk.h> 
using namespace fbxsdk;



class MyFbxManager
{
public:
	//fbxmanager constructor which pulls vertex and index data from an FBX file
	MyFbxManager();
	~MyFbxManager();

public:
	//load a new fbx mesh with skeleton (in a static T pose with no animation)
	std::vector<Mesh_Data> loadFbxMesh(const wchar_t* filename, float* topology);
	//load a new set of skeletons from an FBX file for use with a preexisting mesh
	/* ! It is very important that the animation data file contains a skeleton that is EXACTLY the same as the original model! */
	void loadAnimationData(const wchar_t* filename, Skeletal_Animation& skeletal_animation, u_int sampling_rate = 0);
private:
	void fetchBoneInfluences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& influences, std::map<int, int>& controlpoints, std::vector<VertexMesh>& vertices);
	void fetchBoneMatrices(FbxMesh* fbx_mesh, Skeleton& skeleton, FbxTime time);
	//load animation data skeletons from the fbx mesh and feed them into the skeletalanimation skeleton vector.  
	//!! The sampling rate is natively set to zero because this will cause FBX SDK library to load using the fbx file's native framerate
	void fetchAnimations(FbxMesh* fbx_mesh, Skeletal_Animation& skeletal_animation, u_int sampling_rate = 0);

};

//skeleton structures


