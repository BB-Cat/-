#pragma once
#include "PixelShader.h"
#include "Prerequisites.h"
#include "Shader.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();
	void compileShaders();
	void recompileErrorShaders();
	void recompileShader(int shader);

	void imGuiMenuShaderCompile();

	//function which sets all of the necessary shaders for a specific shader type
	void setPipeline(int shader_type);

	//function which sets the pixel and vertex shader for final pass operations, and makes sure other pipeline stages are removed
	void setFinalPassShader(int final_pass_type);

	//functions used to set individual pieces of the graphics pipeline
	void setPixelShader(int type);
	void setVertexShader(int type = MODEL);
	void setGeometryShader(int type = STANDARD_GS);
	void setHullShader(int type = STANDARD_HS);
	void setDomainShader(int type = STANDARD_DS);

	bool isTessActive() { return m_tess_active; }

private:
	bool m_tess_active; //bool which tells other classes if the domain and hull shaders are currently activated
	int m_active_shader = -1;
private:
	//sprite pixel shader
	PixelShaderPtr m_sprite_ps; //needs revision

	//GEOMETRY SHADERS
	GeometryShaderPtr m_GS_test;
	GeometryShaderPtr m_HullGS;
	GeometryShaderPtr m_GrassShaderGS;

	//HULL SHADERS
	HullShaderPtr m_StandardHS;
	HullShaderPtr m_2DisplaceMapHS;
	HullShaderPtr m_DeferredHS;
	HullShaderPtr m_ShadowMapTerrainHS;
	HullShaderPtr m_TessTerrain3SplatHS;

	//DOMAIN SHADERS
	DomainShaderPtr m_StandardDS;
	DomainShaderPtr m_2DisplaceMapDS;
	DomainShaderPtr m_DeferredDS;
	DomainShaderPtr m_ShadowMapTerrainDS;
	DomainShaderPtr m_TessTerrain3SplatDS;

	//VS PS SHADERS
	ShaderPtr m_error;
	ShaderPtr m_temp;

	ShaderPtr m_flat;
	ShaderPtr m_flat_tex;
	ShaderPtr m_lambert;
	ShaderPtr m_lambert_specular;
	ShaderPtr m_lambert_specular_rimlight;
	ShaderPtr m_texture;
	ShaderPtr m_texture_normal;
	ShaderPtr m_texture_normal_gloss;
	ShaderPtr m_environment_tex;
	ShaderPtr m_geo_test;
	ShaderPtr m_tess_model;
	ShaderPtr m_tess_terrain;
	ShaderPtr m_deferred;
	ShaderPtr m_shadowmap;
	ShaderPtr m_shadowmap_terrain;
	ShaderPtr m_tess_terrain_splat;
	ShaderPtr m_tess_terrain_splat_demo;
	ShaderPtr m_terrain_test;
	ShaderPtr m_terrain_ld;
	ShaderPtr m_terrain_md;
	ShaderPtr m_terrain_hd;
	ShaderPtr m_terrain_ld_toon;
	ShaderPtr m_terrain_md_toon;
	ShaderPtr m_terrain_hd_toon;
	ShaderPtr m_atmosphere;
	ShaderPtr m_white_noise;
	ShaderPtr m_value_noise;
	ShaderPtr m_perlin_noise;
	ShaderPtr m_dynamic_noise;
	ShaderPtr m_perlin_voronoi_noise;
	ShaderPtr m_volume_cloud;
	ShaderPtr m_weather_map;
	ShaderPtr m_weather_atmosphere;
	ShaderPtr m_3D_tex;
	ShaderPtr m_simple_stage;
	ShaderPtr m_triplanar_texture;
	ShaderPtr m_toon_model;
	ShaderPtr m_toon_tex_model;
	ShaderPtr m_gradient;
	ShaderPtr m_hatch;
	ShaderPtr m_debug_grid;

	ShaderPtr m_screenspace;
	ShaderPtr m_screenspace_loadscreen;
	ShaderPtr m_screenspace_selectscreen;

	ShaderPtr m_rt_sphere;
	ShaderPtr m_rt_sphere_clouds;

	//FINAL PASS VS PS SHADERS
	ShaderPtr m_final_basic;
};