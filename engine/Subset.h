#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include "Matrix4X4.h"
#include <DirectXMath.h>
#include <d3d11.h>


class Material_Obj
{
public:
	std::wstring m_name;

	float		m_shininess; //specular power
	float		m_rim_power; //rimlight power
	float		m_transparency;	//transparency 
	float		m_metallic; //transmission filter
	Vector4D	m_ambient_color; //ambient color
	Vector4D	m_diffuse_color; //diffuse color
	Vector4D	m_specular_color; //specular color
	Vector4D	m_emit_color; //emissive color
	Vector4D    m_rim_color; //rimlight color

	TexturePtr m_ambient; //ambient color map
	TexturePtr m_diffuse; //diffuse color map
	TexturePtr m_specular; //specular color map
	TexturePtr m_bump; //bump map
	TexturePtr m_emission; //emission map
	TexturePtr m_height; //pixel heightmap
};


class Subset_Obj
{
public:
	unsigned int m_index_start;
	std::wstring m_mtl;
};


//TODO : search for possible memory leak here
//is this causing a memory leak?
struct Mesh_Material 
{ 
	Vector4D m_color = Vector4D(0.8f, 0.8f, 0.8f, 1.0f);   
	ID3D11ShaderResourceView* shader_resource_view; 
	TexturePtr m_map; 
	float m_extra_params = 0;
};  

struct Mesh_Subset  
{   
	unsigned int index_start = 0; // start number of index buffer   
	unsigned int index_count = 0; // number of vertices (indices)   
	Mesh_Material diffuse;
	Mesh_Material specular;
	Mesh_Material emit;

	TexturePtr m_map_normal;		//bump map
	float m_bumpiness;				//amount of normal map change
	TexturePtr m_map_displacement;	//displacement map
}; 


struct Bone_Influence
{
	int index; // index of bone   
	float weight; // weight of bone  
};

typedef std::vector<Bone_Influence> bone_influences_per_control_point;

struct Bone
{
	Matrix4x4 transform;
};

struct Skeleton 
{
	std::vector<Bone> m_bones;
};

struct Skeletal_Animation : public std::vector<Skeleton>
{ 
	float sampling_time = 1 / 24.0f;    
	float animation_tick = 0.0f; 
};

struct Mesh_Data
{
	VertexBufferPtr						m_vertex_buffer;
	IndexBufferPtr						m_index_buffer;
	std::vector<Mesh_Subset>			m_subs;
	Matrix4x4							m_mesh_world;
	//SkeletalAnimation					m_skeletons;
};

