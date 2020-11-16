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
	float		m_metallicAmount; //transmission filter
	Vector4D	m_ambient_color; //ambient color
	Vector4D	m_diffuse_color; //diffuse color
	Vector4D	m_specular_color; //specular color
	Vector4D	m_emitColor; //emissive color
	Vector4D    m_rim_color; //rimlight color

	float illum; //illumination model

	TexturePtr m_map_Ka; //ambient color map
	TexturePtr m_map_Kd; //diffuse color map
	TexturePtr m_map_Ks; //specular color map
	TexturePtr m_map_bump; //bump map
	TexturePtr m_map_emit; //emission map
	TexturePtr m_map_height; //pixel heightmap
};


class Subset_Obj
{
public:
	unsigned int m_index_start;
	std::wstring m_mtl;
};


struct Material_FBX 
{ 
	Vector4D m_color = Vector4D(0.8f, 0.8f, 0.8f, 1.0f);   
	ID3D11ShaderResourceView* shader_resource_view; 
	TexturePtr m_map; 
	float m_extra_params = 0;
};  

struct Subset_FBX  
{   
	unsigned int index_start = 0; // start number of index buffer   
	unsigned int index_count = 0; // number of vertices (indices)   
	Material_FBX diffuse;
	Material_FBX specular;
	Material_FBX emit;

	TexturePtr m_map_normal;		//bump map
	float m_bumpiness;				//amount of normal map change
	TexturePtr m_map_displacement;	//displacement map
}; 


struct bone_influence
{
	int index; // index of bone   
	float weight; // weight of bone  
};

typedef std::vector<bone_influence> bone_influences_per_control_point;

struct bone
{
	Matrix4x4 transform;
};

struct Skeleton {
	std::vector<bone> m_bones;
};

struct SkeletalAnimation : public std::vector<Skeleton>
{ 
	float sampling_time = 1 / 24.0f;    
	float animation_tick = 0.0f; 
};

struct MeshData
{
	VertexBufferPtr						m_vertex_buffer;
	IndexBufferPtr						m_index_buffer;
	std::vector<Subset_FBX>				m_subs;
	Matrix4x4							m_mesh_world;
	//SkeletalAnimation					m_skeletons;
};

