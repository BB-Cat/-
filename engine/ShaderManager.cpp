#include "ShaderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "GeometryShader.h"

//temporary - array of shader settings stored as ints to simplify switching shaders
int shader_settings[][5] =
{
	//VS, PS, HS, DS, GS
	{FLAT_VS,						FLAT_PS,						HSNONE,					DSNONE,					GSNONE},  		//FLAT,
	{FLAT_VS,						FLAT_TEX_PS,					HSNONE,					DSNONE,					GSNONE},  		//FLAT TEXTURE,
	{LAMBERT_VS,					LAMBERT_PS,						HSNONE,					DSNONE,					GSNONE},		//LAMBERT,
	{LAMBERT_SPECULAR_VS,			LAMBERT_SPECULAR_PS,			HSNONE,					DSNONE,					GSNONE},  		//LAMBERT_SPECULAR,
	{LAMBERT_RIMLIGHT_VS,			LAMBERT_RIMLIGHT_PS,			HSNONE,					DSNONE,					GSNONE},		//LAMBERT_RIMLIGHT,
	{TEXTURE_VS,					TEXTURE_PS,						HSNONE,					DSNONE,					GSNONE},  		//TEXTURE,
	{TEXTURE_NORMAL_VS,				TEXTURE_NORMAL_PS,				HSNONE,					DSNONE,					GSNONE},		//TEXTURE_NORMAL,
	{TEXTURE_NORMAL_GLOSS_VS,		TEXTURE_NORMAL_GLOSS_PS,		HSNONE,					DSNONE,					GSNONE},  		//TEXTURE_NORMAL_GLOSS,
	{TEXTURE_ENVIRONMENT_VS,		TEXTURE_ENVIRONMENT_PS,			HSNONE,					DSNONE,					GSNONE},		//TEXTURE_ENVIRONMENT,
	//{GEO_TEST_VS,					GEO_TEST_PS,					HSNONE,					DSNONE,					STANDARD_GS},	//GEO_TEST,
	{FLAT_VS,						FLAT_PS,						HSNONE,					DSNONE,					GSNONE},  		//FLAT,
	{TEXTURE_TESS_MODEL_VS,			TEXTURE_TESS_MODEL_PS,			STANDARD_HS,			STANDARD_DS,			GSNONE},		//TEXTURE_TESS_MODEL,
	{TEXTURE_TESS_TERRAIN_VS,		TEXTURE_TESS_TERRAIN_PS,		STANDARD_HS,			STANDARD_DS,			GSNONE},  		//TEXTURE_TESS_TERRAIN,
	{TEXTURE_TESS_FLUID_TERRAIN_VS,	TEXTURE_TESS_TERRAIN_PS,		_2DISPLACE_HS,			_2DISPLACE_DS,			GSNONE},		//TEXTURE_TESS_FLUID_TERRAIN,
	{DEFERRED1_VS,					DEFERRED1_PS,					DEFERRED1_HS,			DEFERRED1_DS,			GSNONE},		//DEFERRED RENDERING 1,
	{SHADOWMAP_VS,					SHADOWMAP_PS,					HSNONE,					DSNONE,					GSNONE},  		//SHADOWMAP FOR MODELS,
	{SHADOWMAP_TERRAIN_VS,			SHADOWMAP_PS,					SHADOWMAP_TERRAIN_HS,	SHADOWMAP_TERRAIN_DS,	GSNONE},  		//SHADOWMAP FOR TERRAIN,
	{TEXTURE_TESS_3SPLAT_VS,		TEXTURE_TESS_3SPLAT_PS,			TEXTURE_TESS_3SPLAT_HS, TEXTURE_TESS_3SPLAT_DS,	GSNONE},		//3SPLAT TERRAIN,
	{TEXTURE_TESS_3SPLAT_VS,		TEXTURE_TESS_3SPLAT_PS,			TEXTURE_TESS_3SPLAT_HS, TEXTURE_TESS_3SPLAT_DS,	GRASS_GS},		//3SPLAT TERRAIN WITH GRASS,
	{TERRAIN_TEST_VS,				TERRAIN_TEST_PS,				HSNONE,					DSNONE,					GSNONE},		//TERRAIN TESTING SHADERS
	{TERRAIN_LD_VS,					TERRAIN_LD_PS,					HSNONE,					DSNONE,					GSNONE},		//LOW RES TERRAIN SHADER
	{TERRAIN_MD_VS,					TERRAIN_MD_PS,					HSNONE,					DSNONE,					GSNONE},		//MID RES TERRAIN SHADER
	{TERRAIN_LD_TOON_VS,			TERRAIN_LD_PS,					HSNONE,					DSNONE,					GSNONE},		//LD TOON TERRAIN SHADER
	{TERRAIN_MD_VS,					TERRAIN_MD_TOON_PS,				HSNONE,					DSNONE,					GSNONE},		//MID RES TERRAIN SHADER
	{TEXTURE_TESS_3SPLAT_VS,		TERRAIN_HD_TOON_PS,				TEXTURE_TESS_3SPLAT_HS, TEXTURE_TESS_3SPLAT_DS,	GSNONE},		//HD TOON TERRAIN SHADER
	{ATMOSPHERE_VS,					ATMOSPHERE_PS,					HSNONE,					DSNONE,					GSNONE},		//ATMOSPHERE SHADER
	{TEXTURE_TESS_3SPLAT_DEMO_VS,	TERRAIN_HD_TOON_PS,				TEXTURE_TESS_3SPLAT_HS, TEXTURE_TESS_3SPLAT_DS,	GSNONE},		//HD TOON TERRAIN SHADER
	{WHITE_NOISE_VS,				WHITE_NOISE_PS,					HSNONE,					DSNONE,					GSNONE},		//WHITE NOISE SHADER
	{VALUE_NOISE_VS,				VALUE_NOISE_PS,					HSNONE,					DSNONE,					GSNONE},		//VALUE NOISE SHADER
	{PERLIN_NOISE_VS,				PERLIN_NOISE_PS,				HSNONE,					DSNONE,					GSNONE},		//PERLIN NOISE SHADER
	{DYNAMIC_NOISE_VS,				DYNAMIC_NOISE_PS,				HSNONE,					DSNONE,					GSNONE},		//PERLIN NOISE SHADER
	{PERLIN_VORONOI_NOISE_VS,		PERLIN_VORONOI_NOISE_PS,		HSNONE,					DSNONE,					GSNONE},		//PERLIN NOISE SHADER
	{VOLUME_CLOUD_VS,				VOLUME_CLOUD_PS,				HSNONE,					DSNONE,					GSNONE},		//CLOUD SHADER
	{_3DTEX_VS,						_3DTEX_PS,						HSNONE,					DSNONE,					GSNONE},		//SHADER TO CHECK 3D TEXTURE FUNCTIONALITY
	{WEATHER_MAP_VS,				WEATHER_MAP_PS,					HSNONE,					DSNONE,					GSNONE},		//SHADER FOR GENERATING WEATHER
	{WEATHER_ATMOSPHERE_VS,			WEATHER_ATMOSPHERE_PS,			HSNONE,					DSNONE,					GSNONE},		//ATMOSPHERE SHADER WITH WEATHER
	{SIMPLE_STAGE_VS,				SIMPLE_STAGE_PS,				HSNONE,					DSNONE,					GSNONE},		//SHADER FOR PLAYER MOVEMENT SCENE GROUND
};

ShaderManager::ShaderManager() : m_tess_active(false)
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::compileShaders()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	//compile sprite shader
	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"newPS.hlsl", "pssprite", &shader_byte_code, &size_shader);
	m_sprite_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	//***************************************************************************//
	//***************************************************************************//
	//   UPDATED SHADERS - DO NOT USE PREVIOUS SHADERS                           //
	//***************************************************************************//


	//flat shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"FlatShaderVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_FlatVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"FlatShaderPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_FlatPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	
	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"FlatTexShaderPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_FlatTexPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//lambert shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"LambertVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_LambertVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"LambertPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_LambertPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//specular shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"SpecularVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_Lambert_SpecularVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SpecularPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_Lambert_SpecularPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//rimlight shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"RimlightVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_Lambert_Specular_RimlightVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"RimlightPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_Lambert_Specular_RimlightPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//textured rimlight shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TexturedRimlightVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TextureVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TexturedRimlightPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TexturePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//textured normal map shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TexturedNormalMapVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TextureNormalVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TexturedNormalMapPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TextureNormalPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//textured normal map shader with specular map
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"GlossVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TextureNormalGlossVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"GlossPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TextureNormalGlossPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//textured shader with environmental reflection texture map
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"EnvironmentTexMapVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_EnvTexVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"EnvironmentTexMapPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_EnvTexPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

		//copy of textured rimlight vs to test geometry shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"GeometryTestVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_GeoTestVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"GeometryTestPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_GeoTestPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//textured shader which tesselates and uses a displacement map for dynamic detail LOD for models
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TessModelVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TessModelVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TessModelPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TessModelPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//textured shader which tesselates and uses a displacement map for dynamic detail LOD modified for terrain
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TessTerrainVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TessTerrainVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TessFluidTerrainVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TessFluidTerrainVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TessTerrainPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TessTerrainPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//sample shaders for deferred rendering
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"DeferredVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_DeferredVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"DeferredPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_DeferredPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//shader for directional light shadowmapping
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"DirLightShadowMapVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_ShadowMapVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"DirLightShadowMapTessVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_ShadowMapTerrainVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"DirLightShadowMapPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ShadowMapPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//shader for texture splat terrain
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"Terrain3SplatVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TessTerrain3SplatVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"Terrain3SplatDemoVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TessTerrain3SplatDemoVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"Terrain3SplatPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TessTerrain3SplatPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//shader for troubleshooting terrain
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TerrainTestVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TerrainTestVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TerrainTestPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TerrainTestPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//shader for low definition terrain
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"LDTerrainVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TerrainLDVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"LDTerrainPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TerrainLDPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//shader for mid definition terrain
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"MDTerrainVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TerrainMDVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"MDTerrainPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TerrainMDPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//shaders for HD toon terrain
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"LDTerrainVSToon.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_TerrainLDVS_toon = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"MDTerrainPSToon.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TerrainMDPS_toon = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"HDTerrainPSToon.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_TerrainHDPS_toon = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//shader for dynamic skybox
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"AtmosphericShaderVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_AtmosphereVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"AtmosphericShaderPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_AtmospherePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//white noise shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"WhiteNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_WhiteNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"WhiteNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_WhiteNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//value noise shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"ValueNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_ValueNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"ValueNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ValueNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//perlin noise shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"PerlinNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_PerlinNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PerlinNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_PerlinNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//perlin voronoi noise shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"PerlinVoronoiNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_PerlinVoronoiNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PerlinVoronoiNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_PerlinVoronoiNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//dynamic noise shader for runtime adjustments before exporting texture data
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"DynamicNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_DynamicNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"DynamicNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_DynamicNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//volumetric cloud shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"CloudVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_VolumeCloudVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"CloudPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_VolumeCloudPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//3D Texture confirmation shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"3DTexTestVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_3DTexVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"3DTexTestPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_3DTexPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//Shader for generating weather maps used for cloudscapes
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"WeatherMapVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_WeatherMapVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"WeatherMapPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_WeatherMapPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//Atmosphere shader with added clouds based on the weathermap shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"WeatherAtmosphereVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_WeatherAtmosphereVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"WeatherAtmospherePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_WeatherAtmospherePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	//Atmosphere shader with added clouds based on the weathermap shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"SimpleStageVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_SimpleStageVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SimpleStagePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_SimpleStagePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//


	//***************************************************************************//
	//   GEOMETRY SHADERS							                             //
	//***************************************************************************//
	GraphicsEngine::get()->getRenderSystem()->compileGeometryShader(L"GeometryShader.hlsl", "gsmain", &shader_byte_code, &size_shader);
	m_GS_test = GraphicsEngine::get()->getRenderSystem()->createGeometryShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileGeometryShader(L"GrassShaderGS.hlsl", "gsmain", &shader_byte_code, &size_shader);
	m_GrassShaderGS = GraphicsEngine::get()->getRenderSystem()->createGeometryShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//***************************************************************************//
	//   HULL SHADERS															 //
	//***************************************************************************//
	GraphicsEngine::get()->getRenderSystem()->compileHullShader(L"DefaultHS.hlsl", "hsmain", &shader_byte_code, &size_shader);
	m_StandardHS = GraphicsEngine::get()->getRenderSystem()->createHullShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileHullShader(L"2DisplacementHS.hlsl", "hsmain", &shader_byte_code, &size_shader);
	m_2DisplaceMapHS = GraphicsEngine::get()->getRenderSystem()->createHullShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileHullShader(L"DeferredHS.hlsl", "hsmain", &shader_byte_code, &size_shader);
	m_DeferredHS = GraphicsEngine::get()->getRenderSystem()->createHullShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileHullShader(L"DirLightShadowMapTessHS.hlsl", "hsmain", &shader_byte_code, &size_shader);
	m_ShadowMapTerrainHS = GraphicsEngine::get()->getRenderSystem()->createHullShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileHullShader(L"Terrain3SplatHS.hlsl", "hsmain", &shader_byte_code, &size_shader);
	m_TessTerrain3SplatHS = GraphicsEngine::get()->getRenderSystem()->createHullShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//
	//   DOMAIN SHADERS															 //
	//***************************************************************************//
	GraphicsEngine::get()->getRenderSystem()->compileDomainShader(L"DefaultDS.hlsl", "dsmain", &shader_byte_code, &size_shader);
	m_StandardDS = GraphicsEngine::get()->getRenderSystem()->createDomainShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileDomainShader(L"2DisplacementDS.hlsl", "dsmain", &shader_byte_code, &size_shader);
	m_2DisplaceMapDS = GraphicsEngine::get()->getRenderSystem()->createDomainShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileDomainShader(L"DeferredDS.hlsl", "dsmain", &shader_byte_code, &size_shader);
	m_DeferredDS = GraphicsEngine::get()->getRenderSystem()->createDomainShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileDomainShader(L"DirLightShadowMapTessDS.hlsl", "dsmain", &shader_byte_code, &size_shader);
	m_ShadowMapTerrainDS = GraphicsEngine::get()->getRenderSystem()->createDomainShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileDomainShader(L"Terrain3SplatDS.hlsl", "dsmain", &shader_byte_code, &size_shader);
	m_TessTerrain3SplatDS = GraphicsEngine::get()->getRenderSystem()->createDomainShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	//***************************************************************************//
	//   FINAL PASS SHADERS														 //
	//***************************************************************************//

	//sample shaders for deferred rendering
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"FinalPassBasicVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_FinalBasicVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"FinalPassBasicPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_FinalBasicPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
}

void ShaderManager::setPipeline(int shader_type)
{
	setVertexShader(shader_settings[shader_type][0]);
	setPixelShader(shader_settings[shader_type][1]);
	setHullShader(shader_settings[shader_type][2]);
	setDomainShader(shader_settings[shader_type][3]);
	setGeometryShader(shader_settings[shader_type][4]);
}

void ShaderManager::setFinalPassShader(int final_pass_type)
{
	switch (final_pass_type)
	{
	case FinalPassShaders::TEST:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_FinalBasicVS);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_FinalBasicPS);
		break;
	}

	//remove other pipeline stages
	setHullShader(HSNONE);
	setDomainShader(DSNONE);
	setGeometryShader(GSNONE);
}

void ShaderManager::setPixelShader(int type)
{
	switch (type)
	{
	case SPRITE:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_sprite_ps);
		break;
	case FLAT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_FlatPS);
		break;
	case FLAT_TEX_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_FlatTexPS);
		break;
	case LAMBERT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_LambertPS);
		break;
	case LAMBERT_SPECULAR_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_Lambert_SpecularPS);
		break;
	case LAMBERT_RIMLIGHT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_Lambert_Specular_RimlightPS);
		break;
	case TEXTURE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TexturePS);
		break;
	case TEXTURE_NORMAL_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TextureNormalPS);
		break;
	case TEXTURE_NORMAL_GLOSS_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TextureNormalGlossPS);
		break;
	case TEXTURE_ENVIRONMENT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_EnvTexPS);
		break;
	case GEO_TEST_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_GeoTestPS);
		break;
	case TEXTURE_TESS_MODEL_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TessModelPS);
		break;
	case TEXTURE_TESS_TERRAIN_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TessTerrainPS);
		break;
	case DEFERRED1_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_DeferredPS);
		break;
	case SHADOWMAP_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ShadowMapPS);
		break;
	case TEXTURE_TESS_3SPLAT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TessTerrain3SplatPS);
		break;
	case TERRAIN_TEST_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TerrainTestPS);
		break;
	case TERRAIN_LD_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TerrainLDPS);
		break;
	case TERRAIN_MD_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TerrainMDPS);
		break;
	case TERRAIN_MD_TOON_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TerrainMDPS_toon);
		break;
	case TERRAIN_HD_TOON_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_TerrainHDPS_toon);
		break;
	case ATMOSPHERE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_AtmospherePS);
		break;
	case WHITE_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_WhiteNoisePS);
		break;
	case VALUE_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ValueNoisePS);
		break;
	case PERLIN_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_PerlinNoisePS);
		break;
	case DYNAMIC_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_DynamicNoisePS);
		break;
	case PERLIN_VORONOI_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_PerlinVoronoiNoisePS);
		break;
	case VOLUME_CLOUD_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_VolumeCloudPS);
		break;
	case _3DTEX_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_3DTexPS);
		break;
	case WEATHER_MAP_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_WeatherMapPS);
		break;
	case WEATHER_ATMOSPHERE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_WeatherAtmospherePS);
		break;
	case SIMPLE_STAGE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_SimpleStagePS);
		break;
	}
}

void ShaderManager::setVertexShader(int type)
{
	switch (type)
	{
	case FLAT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_FlatVS);
		break;
	case LAMBERT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_LambertVS);
		break;
	case LAMBERT_SPECULAR_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_Lambert_SpecularVS);
		break;
	case LAMBERT_RIMLIGHT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_Lambert_Specular_RimlightVS);
		break;
	case TEXTURE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TextureVS);
		break;
	case TEXTURE_NORMAL_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TextureNormalVS);
		break;
	case TEXTURE_NORMAL_GLOSS_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TextureNormalGlossVS);
		break;
	case TEXTURE_ENVIRONMENT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_EnvTexVS);
		break;
	case GEO_TEST_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_GeoTestVS);
		break;
	case TEXTURE_TESS_MODEL_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TessModelVS);
		break;
	case TEXTURE_TESS_TERRAIN_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TessTerrainVS);
		break;
	case TEXTURE_TESS_FLUID_TERRAIN_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TessFluidTerrainVS);
		break;
	case DEFERRED1_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_DeferredVS);
		break;
	case SHADOWMAP_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_ShadowMapVS);
		break;
	case SHADOWMAP_TERRAIN_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_ShadowMapTerrainVS);
		break;
	case TEXTURE_TESS_3SPLAT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TessTerrain3SplatVS);
		break;
	case TERRAIN_TEST_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TerrainTestVS);
		break;
	case TERRAIN_LD_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TerrainLDVS);
		break;
	case TERRAIN_MD_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TerrainMDVS);
		break;
	case TERRAIN_LD_TOON_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TerrainLDVS_toon);
		break;
	case ATMOSPHERE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_AtmosphereVS);
		break;
	case TEXTURE_TESS_3SPLAT_DEMO_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TessTerrain3SplatDemoVS);
		break;
	case WHITE_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_WhiteNoiseVS);
		break;
	case VALUE_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_ValueNoiseVS);
		break;
	case PERLIN_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_PerlinNoiseVS);
		break;
	case DYNAMIC_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_DynamicNoiseVS);
		break;
	case PERLIN_VORONOI_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_PerlinVoronoiNoiseVS);
		break;
	case VOLUME_CLOUD_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_VolumeCloudVS);
		break;
	case _3DTEX_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_3DTexVS);
		break;
	case WEATHER_MAP_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_WeatherMapVS);
		break;
	case WEATHER_ATMOSPHERE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_WeatherAtmosphereVS);
		break;
	case SIMPLE_STAGE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_SimpleStageVS);
		break;
	}
}

void ShaderManager::setGeometryShader(int type)
{
	switch (type)
	{
	case GSNONE:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->removeGeometryShader();
		break;
	case STANDARD_GS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setGeometryShader(m_GS_test);
		break;
	case HULL_GS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setGeometryShader(m_HullGS);
		break;
	case GRASS_GS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setGeometryShader(m_GrassShaderGS);
		break;
	}
}

void ShaderManager::setHullShader(int type)
{
	switch (type)
	{
	case HSNONE: 
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->removeHullShader();
		m_tess_active = false;
		break;
	case STANDARD_HS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setHullShader(m_StandardHS);
		m_tess_active = true;
		break;
	case _2DISPLACE_HS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setHullShader(m_2DisplaceMapHS);
		m_tess_active = true;
		break;
	case DEFERRED1_HS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setHullShader(m_DeferredHS);
		m_tess_active = true;
		break;
	case SHADOWMAP_TERRAIN_HS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setHullShader(m_ShadowMapTerrainHS);
		m_tess_active = true;
		break;
	case TEXTURE_TESS_3SPLAT_HS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setHullShader(m_TessTerrain3SplatHS);
		m_tess_active = true;
		break;
	}
}

void ShaderManager::setDomainShader(int type)
{
	switch (type)
	{
	case DSNONE:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->removeDomainShader();
		break;
	case STANDARD_DS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDomainShader(m_StandardDS);
		break;
	case _2DISPLACE_DS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDomainShader(m_2DisplaceMapDS);
		break;
	case DEFERRED1_DS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDomainShader(m_DeferredDS);
		break;
	case SHADOWMAP_TERRAIN_DS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDomainShader(m_ShadowMapTerrainDS);
		break;
	case TEXTURE_TESS_3SPLAT_DS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setDomainShader(m_TessTerrain3SplatDS);
		break;
	}
}