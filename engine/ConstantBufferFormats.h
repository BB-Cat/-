#pragma once
#include "Prerequisites.h"
#include "Matrix4X4.h"
#include "Vector4D.h"
#include <DirectXMath.h>

__declspec(align(16))
struct cb_world
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_projection;
	Vector4D m_camera_position;
};

#define NUM_SHADOW_MAPS (3)

__declspec(align(16))
struct cb_world_dirlight
{
	Vector4D m_active_proj; //this value is for the shadow map pass to know which projection matrix to use.
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_projection[NUM_SHADOW_MAPS];
	
};



__declspec(align(16))
struct cb_local
{
	Matrix4x4 m_local;
	Matrix4x4 m_bone_transforms[MAXBONES];
	Matrix4x4 m_coord_conversion;
};

__declspec(align(16))
struct cb_mesh_light_properties
{
	float		m_shininess; //specular power
	float		m_rimPower; //rimlight power
	float		m_transparency;	//transparency 
	float		m_metallicAmount; //amount of metallicism for environment reflections
	Vector4D	m_ambientColor; //ambient color
	Vector4D	m_diffuseColor; //diffuse color
	Vector4D	m_specularColor; //specular color
	Vector4D	m_emitColor; //emissive color
	Vector4D    m_rimColor; //rimlight color
};

__declspec(align(16))
struct cb_scene_lighting
{
	Vector3D m_global_light_dir;
	float	 m_global_light_strength;
	Vector4D m_global_light_color;
	Vector4D m_ambient_light_color;
};

__declspec(align(16))
struct cb_vs_terrainLD
{
	Vector4D m_c1; //average color of terrain type 1
	Vector4D m_c2; //average color of terrain type 2
	Vector4D m_c3; //average color of terrain type 3
};


#define NUM_LIGHTS (20)
__declspec(align(16))
struct cb_lights
{
	Vector4D m_light_pos[NUM_LIGHTS];
	Vector4D m_light_color[NUM_LIGHTS];
	Vector4D m_light_strength[NUM_LIGHTS];
};


__declspec(align(16))
struct cb_tess
{
	float m_max_tess;
	float m_min_tess;
	float m_min_tess_range;
	float m_max_tess_range;
};

__declspec(align(16))
struct cb_time
{
	float m_time; //millisecond timer
	float m_elapsed; //time since the last frame
};

__declspec(align(16))
struct cb_ds_height
{
	float m_height; //displacement value for DS
};


__declspec(align(16))
struct cb_noise
{
	//x - white,  y - value,  z - perlin,  w - voronoi
	Vector4D m_noise_type;
	Vector4D m_show_rgba;

	float m_vor_octaves;
	float m_vor_frequency;
	float m_vor_gain;
	float m_vor_lacunarity;
	float m_vor_amplitude;
	float m_vor_cell_size;

	float m_per_octaves;
	float m_per_frequency;
	float m_per_gain;
	float m_per_lacunarity;
	float m_per_amplitude;
	float m_per_cell_size;
};

__declspec(align(16))
struct cb_cloud
{
	Vector4D	m_cloud_position;
	Vector4D	m_cloud_size;
	Vector4D	m_sampling_resolution;
	Vector4D	m_sampling_weight;
	Vector4D    m_move_dir;
	float		m_cloud_density;
	float		m_vertical_fade;
	float		m_horizontal_fade;
	float		m_per_pixel_fade_threshhold;
	float		m_per_sample_fade_threshhold;
	float		m_in_scattering_strength;
	float		m_out_scattering_strength;
	float		m_time;
	float		m_speed;
};
