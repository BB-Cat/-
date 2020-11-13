#pragma once
#include "PixelShader.h"


class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();
	void compileShaders();

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
	VertexShaderPtr m_FlatVS;
	PixelShaderPtr m_FlatPS;
	PixelShaderPtr m_FlatTexPS;

	VertexShaderPtr m_LambertVS;
	PixelShaderPtr m_LambertPS;

	VertexShaderPtr m_Lambert_SpecularVS;
	PixelShaderPtr m_Lambert_SpecularPS;

	VertexShaderPtr m_Lambert_Specular_RimlightVS;
	PixelShaderPtr m_Lambert_Specular_RimlightPS;

	VertexShaderPtr m_TextureVS;
	PixelShaderPtr m_TexturePS;

	VertexShaderPtr m_TextureNormalVS;
	PixelShaderPtr m_TextureNormalPS;

	VertexShaderPtr m_TextureNormalGlossVS;
	PixelShaderPtr m_TextureNormalGlossPS;

	VertexShaderPtr m_EnvTexVS;
	PixelShaderPtr m_EnvTexPS;

	VertexShaderPtr m_GeoTestVS;
	PixelShaderPtr m_GeoTestPS;

	VertexShaderPtr m_TessModelVS;
	PixelShaderPtr m_TessModelPS;

	VertexShaderPtr m_TessTerrainVS;
	VertexShaderPtr m_TessFluidTerrainVS;
	PixelShaderPtr m_TessTerrainPS;

	VertexShaderPtr m_DeferredVS;
	PixelShaderPtr m_DeferredPS;

	VertexShaderPtr m_ShadowMapVS;
	VertexShaderPtr m_ShadowMapTerrainVS;
	PixelShaderPtr m_ShadowMapPS;

	VertexShaderPtr m_TessTerrain3SplatVS;
	VertexShaderPtr m_TessTerrain3SplatDemoVS;
	PixelShaderPtr m_TessTerrain3SplatPS;

	VertexShaderPtr m_TerrainTestVS;
	PixelShaderPtr m_TerrainTestPS;

	VertexShaderPtr m_TerrainLDVS;
	PixelShaderPtr m_TerrainLDPS;

	VertexShaderPtr m_TerrainMDVS;
	PixelShaderPtr m_TerrainMDPS;

	VertexShaderPtr m_TerrainHDVS;
	PixelShaderPtr m_TerrainHDPS;

	VertexShaderPtr m_TerrainLDVS_toon;
	PixelShaderPtr m_TerrainMDPS_toon;
	PixelShaderPtr m_TerrainHDPS_toon;

	VertexShaderPtr m_AtmosphereVS;
	PixelShaderPtr m_AtmospherePS;

	VertexShaderPtr m_WhiteNoiseVS;
	PixelShaderPtr m_WhiteNoisePS;

	VertexShaderPtr m_ValueNoiseVS;
	PixelShaderPtr m_ValueNoisePS;

	VertexShaderPtr m_PerlinNoiseVS;
	PixelShaderPtr m_PerlinNoisePS;

	VertexShaderPtr m_DynamicNoiseVS;
	PixelShaderPtr m_DynamicNoisePS;

	VertexShaderPtr m_PerlinVoronoiNoiseVS;
	PixelShaderPtr m_PerlinVoronoiNoisePS;

	VertexShaderPtr m_VolumeCloudVS;
	PixelShaderPtr m_VolumeCloudPS;

	VertexShaderPtr m_WeatherMapVS;
	PixelShaderPtr m_WeatherMapPS;

	VertexShaderPtr m_WeatherAtmosphereVS;
	PixelShaderPtr m_WeatherAtmospherePS;

	VertexShaderPtr m_3DTexVS;
	PixelShaderPtr m_3DTexPS;

	VertexShaderPtr m_SimpleStageVS;
	PixelShaderPtr m_SimpleStagePS;

	//FINAL PASS VS PS SHADERS
	VertexShaderPtr m_FinalBasicVS;
	PixelShaderPtr m_FinalBasicPS;
};