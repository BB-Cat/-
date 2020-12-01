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
	//VertexShaderPtr m_ErrorVS;
	//PixelShaderPtr m_ErrorPS;
	ShaderPtr m_error;

	ShaderPtr m_temp;

	//VertexShaderPtr m_FlatVS;
	//PixelShaderPtr m_FlatPS;
	ShaderPtr m_flat;
	//PixelShaderPtr m_FlatTexPS;
	ShaderPtr m_flat_tex;

	//VertexShaderPtr m_LambertVS;
	//PixelShaderPtr m_LambertPS;
	ShaderPtr m_lambert;

	//VertexShaderPtr m_Lambert_SpecularVS;
	//PixelShaderPtr m_Lambert_SpecularPS;
	ShaderPtr m_lambert_specular;

	//VertexShaderPtr m_Lambert_Specular_RimlightVS;
	//PixelShaderPtr m_Lambert_Specular_RimlightPS;
	ShaderPtr m_lambert_specular_rimlight;

	//VertexShaderPtr m_TextureVS;
	//PixelShaderPtr m_TexturePS;
	ShaderPtr m_texture;

	//VertexShaderPtr m_TextureNormalVS;
	//PixelShaderPtr m_TextureNormalPS;
	ShaderPtr m_texture_normal;

	//VertexShaderPtr m_TextureNormalGlossVS;
	//PixelShaderPtr m_TextureNormalGlossPS;
	ShaderPtr m_texture_normal_gloss;

	//VertexShaderPtr m_EnvTexVS;
	//PixelShaderPtr m_EnvTexPS;
	ShaderPtr m_environment_tex;

	//VertexShaderPtr m_GeoTestVS;
	//PixelShaderPtr m_GeoTestPS;
	ShaderPtr m_geo_test;

	//VertexShaderPtr m_TessModelVS;
	//PixelShaderPtr m_TessModelPS;
	ShaderPtr m_tess_model;

	//VertexShaderPtr m_TessTerrainVS;
	//VertexShaderPtr m_TessFluidTerrainVS;
	//PixelShaderPtr m_TessTerrainPS;
	ShaderPtr m_tess_terrain;

	//VertexShaderPtr m_DeferredVS;
	//PixelShaderPtr m_DeferredPS;
	ShaderPtr m_deferred;

	//VertexShaderPtr m_ShadowMapVS;
	//VertexShaderPtr m_ShadowMapTerrainVS;
	//PixelShaderPtr m_ShadowMapPS;
	ShaderPtr m_shadowmap;
	ShaderPtr m_shadowmap_terrain;

	//VertexShaderPtr m_TessTerrain3SplatVS;
	//VertexShaderPtr m_TessTerrain3SplatDemoVS;
	//PixelShaderPtr m_TessTerrain3SplatPS;
	ShaderPtr m_tess_terrain_splat;
	ShaderPtr m_tess_terrain_splat_demo;

	//VertexShaderPtr m_TerrainTestVS;
	//PixelShaderPtr m_TerrainTestPS;
	ShaderPtr m_terrain_test;

	//VertexShaderPtr m_TerrainLDVS;
	//PixelShaderPtr m_TerrainLDPS;
	ShaderPtr m_terrain_ld;

	//VertexShaderPtr m_TerrainMDVS;
	//PixelShaderPtr m_TerrainMDPS;
	ShaderPtr m_terrain_md;

	//VertexShaderPtr m_TerrainHDVS;
	//PixelShaderPtr m_TerrainHDPS;
	ShaderPtr m_terrain_hd;

	//VertexShaderPtr m_TerrainLDVS_toon;
	//PixelShaderPtr m_TerrainMDPS_toon;
	//PixelShaderPtr m_TerrainHDPS_toon;
	ShaderPtr m_terrain_ld_toon;
	ShaderPtr m_terrain_md_toon;
	ShaderPtr m_terrain_hd_toon;

	//VertexShaderPtr m_AtmosphereVS;
	//PixelShaderPtr m_AtmospherePS;
	ShaderPtr m_atmosphere;

	//VertexShaderPtr m_WhiteNoiseVS;
	//PixelShaderPtr m_WhiteNoisePS;
	ShaderPtr m_white_noise;

	//VertexShaderPtr m_ValueNoiseVS;
	//PixelShaderPtr m_ValueNoisePS;
	ShaderPtr m_value_noise;

	//VertexShaderPtr m_PerlinNoiseVS;
	//PixelShaderPtr m_PerlinNoisePS;
	ShaderPtr m_perlin_noise;

	//VertexShaderPtr m_DynamicNoiseVS;
	//PixelShaderPtr m_DynamicNoisePS;
	ShaderPtr m_dynamic_noise;

	//VertexShaderPtr m_PerlinVoronoiNoiseVS;
	//PixelShaderPtr m_PerlinVoronoiNoisePS;
	ShaderPtr m_perlin_voronoi_noise;

	//VertexShaderPtr m_VolumeCloudVS;
	//PixelShaderPtr m_VolumeCloudPS;
	ShaderPtr m_volume_cloud;

	//VertexShaderPtr m_WeatherMapVS;
	//PixelShaderPtr m_WeatherMapPS;
	ShaderPtr m_weather_map;

	//VertexShaderPtr m_WeatherAtmosphereVS;
	//PixelShaderPtr m_WeatherAtmospherePS;
	ShaderPtr m_weather_atmosphere;

	//VertexShaderPtr m_3DTexVS;
	//PixelShaderPtr m_3DTexPS;
	ShaderPtr m_3D_tex;

	//VertexShaderPtr m_SimpleStageVS;
	//PixelShaderPtr m_SimpleStagePS;
	ShaderPtr m_simple_stage;


	ShaderPtr m_triplanar_texture;

	ShaderPtr m_toon_model;

	ShaderPtr m_toon_tex_model;

	ShaderPtr m_gradient;

	ShaderPtr m_hatch;

	//FINAL PASS VS PS SHADERS
	//VertexShaderPtr m_FinalBasicVS;
	//PixelShaderPtr m_FinalBasicPS;
	ShaderPtr m_final_basic;
};