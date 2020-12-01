#include "ShaderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "GeometryShader.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

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
	{TRIPLANAR_TEXTURE_VS,			TRIPLANAR_TEXTURE_PS,			HSNONE,					DSNONE,					GSNONE},		//SHADER FOR 3D TEXTURE MAPPING
	{TOON_MODEL_VS,					TOON_MODEL_PS,					HSNONE,					DSNONE,					GSNONE},		//TOON SHADER FOR MODELS
	{TOON_MODEL_TEX_VS,				TOON_MODEL_TEX_PS,				HSNONE,					DSNONE,					GSNONE},		//TOON SHADER FOR MODELS WITH TEXTURING

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

	m_temp = std::shared_ptr<Shader>(new Shader(L"NewCloudVS.hlsl", L"NewCloudPS.hlsl", shader_byte_code, size_shader));

	//error shader - used when a shader is unable to compile
	m_error = std::shared_ptr<Shader>(new Shader(L"ErrorVS.hlsl", L"ErrorPS.hlsl", shader_byte_code, size_shader));

	//***************************************************************************//

	m_flat = std::shared_ptr<Shader>(new Shader(L"FlatShaderVS.hlsl", L"FlatShaderPS.hlsl", shader_byte_code, size_shader));
	m_flat->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_flat_tex = std::shared_ptr<Shader>(new Shader(L"FlatShaderVS.hlsl", L"FlatTexShaderPS.hlsl", shader_byte_code, size_shader));
	m_flat_tex->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_lambert = std::shared_ptr<Shader>(new Shader(L"LambertVS.hlsl", L"LambertPS.hlsl", shader_byte_code, size_shader));
	m_lambert->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_lambert_specular = std::shared_ptr<Shader>(new Shader(L"SpecularVS.hlsl", L"SpecularPS.hlsl", shader_byte_code, size_shader));
	m_lambert_specular->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_lambert_specular_rimlight = std::shared_ptr<Shader>(new Shader(L"RimlightVS.hlsl", L"RimlightPS.hlsl", shader_byte_code, size_shader));
	m_lambert_specular_rimlight->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_texture = std::shared_ptr<Shader>(new Shader(L"TexturedRimlightVS.hlsl", L"TexturedRimlightPS.hlsl", shader_byte_code, size_shader));
	m_texture->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_texture_normal = std::shared_ptr<Shader>(new Shader(L"TexturedNormalMapVS.hlsl", L"TexturedNormalMapPS.hlsl", shader_byte_code, size_shader));
	m_texture_normal->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_texture_normal_gloss = std::shared_ptr<Shader>(new Shader(L"GlossVS.hlsl", L"GlossPS.hlsl", shader_byte_code, size_shader));
	m_texture_normal_gloss->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_environment_tex = std::shared_ptr<Shader>(new Shader(L"EnvironmentTexMapVS.hlsl", L"EnvironmentTexMapPS.hlsl", shader_byte_code, size_shader));
	m_environment_tex->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_geo_test = std::shared_ptr<Shader>(new Shader(L"GeometryTestVS.hlsl", L"GeometryTestPS.hlsl", shader_byte_code, size_shader));
	m_geo_test->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_tess_model = std::shared_ptr<Shader>(new Shader(L"GeometryTestVS.hlsl", L"GeometryTestPS.hlsl", shader_byte_code, size_shader));
	m_tess_model->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_tess_terrain = std::shared_ptr<Shader>(new Shader(L"TessTerrainVS.hlsl", L"TessTerrainPS.hlsl", shader_byte_code, size_shader));
	m_tess_terrain->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	//***************************************************************************//

	m_deferred = std::shared_ptr<Shader>(new Shader(L"DeferredVS.hlsl", L"DeferredPS.hlsl", shader_byte_code, size_shader));
	m_deferred->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_shadowmap = std::shared_ptr<Shader>(new Shader(L"DirLightShadowMapVS.hlsl", L"DirLightShadowMapPS.hlsl", shader_byte_code, size_shader));
	m_shadowmap->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_shadowmap_terrain = std::shared_ptr<Shader>(new Shader(L"DirLightShadowMapTessVS.hlsl", L"DirLightShadowMapPS.hlsl", shader_byte_code, size_shader));
	m_shadowmap_terrain->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//! because sending a faulty pixel shader will cause the error shader to take over both vs and ps shaders, we are passing an
	//error pixel shader to terrain splat and terrain splat demo to preserve them (terrain3spatPS is not currently compileable)
	m_tess_terrain_splat = std::shared_ptr<Shader>(new Shader(L"Terrain3SplatVS.hlsl", L"ErrorPS.hlsl", shader_byte_code, size_shader));
	m_tess_terrain_splat->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_tess_terrain_splat_demo = std::shared_ptr<Shader>(new Shader(L"Terrain3SplatDemoVS.hlsl", L"ErrorPS.hlsl", shader_byte_code, size_shader));
	m_tess_terrain_splat_demo->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	//***************************************************************************//

	m_terrain_test = std::shared_ptr<Shader>(new Shader(L"TerrainTestVS.hlsl", L"TerrainTestPS.hlsl", shader_byte_code, size_shader));
	m_terrain_test->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_terrain_ld = std::shared_ptr<Shader>(new Shader(L"LDTerrainVS.hlsl", L"LDTerrainPS.hlsl", shader_byte_code, size_shader));
	m_terrain_ld->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_terrain_md = std::shared_ptr<Shader>(new Shader(L"MDTerrainVS.hlsl", L"MDTerrainPS.hlsl", shader_byte_code, size_shader));
	m_terrain_md->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_terrain_ld_toon = std::shared_ptr<Shader>(new Shader(L"LDTerrainVSToon.hlsl", L"LDTerrainPS.hlsl", shader_byte_code, size_shader));
	m_terrain_ld_toon->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_terrain_md_toon = std::shared_ptr<Shader>(new Shader(L"MDTerrainVS.hlsl", L"MDTerrainPSToon.hlsl", shader_byte_code, size_shader));
	m_terrain_md_toon->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_terrain_hd_toon = std::shared_ptr<Shader>(new Shader(L"Terrain3SplatVS.hlsl", L"HDTerrainPSToon.hlsl", shader_byte_code, size_shader));
	m_terrain_hd_toon->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_atmosphere = std::shared_ptr<Shader>(new Shader(L"AtmosphericShaderVS.hlsl", L"AtmosphericShaderPS.hlsl", shader_byte_code, size_shader));
	m_atmosphere->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_white_noise = std::shared_ptr<Shader>(new Shader(L"WhiteNoiseVS.hlsl", L"WhiteNoisePS.hlsl", shader_byte_code, size_shader));
	m_white_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_value_noise = std::shared_ptr<Shader>(new Shader(L"ValueNoiseVS.hlsl", L"ValueNoisePS.hlsl", shader_byte_code, size_shader));
	m_value_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_perlin_noise = std::shared_ptr<Shader>(new Shader(L"PerlinNoiseVS.hlsl", L"PerlinNoisePS.hlsl", shader_byte_code, size_shader));
	m_perlin_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_perlin_voronoi_noise = std::shared_ptr<Shader>(new Shader(L"PerlinVoronoiNoiseVS.hlsl", L"PerlinVoronoiNoisePS.hlsl", shader_byte_code, size_shader));
	m_perlin_voronoi_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_dynamic_noise = std::shared_ptr<Shader>(new Shader(L"DynamicNoiseVS.hlsl", L"DynamicNoisePS.hlsl", shader_byte_code, size_shader));
	m_dynamic_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//volumetric cloud shader
	m_volume_cloud = std::shared_ptr<Shader>(new Shader(L"NewCloudVS.hlsl", L"NewCloudPS.hlsl", shader_byte_code, size_shader));
	m_volume_cloud->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//3D Texture confirmation shader
	m_3D_tex = std::shared_ptr<Shader>(new Shader(L"3DTexTestVS.hlsl", L"3DTexTestPS.hlsl", shader_byte_code, size_shader));
	m_3D_tex->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_weather_map = std::shared_ptr<Shader>(new Shader(L"WeatherMapVS.hlsl", L"WeatherMapPS.hlsl", shader_byte_code, size_shader));
	m_weather_map->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_weather_atmosphere = std::shared_ptr<Shader>(new Shader(L"WeatherAtmosphereVS.hlsl", L"WeatherAtmospherePS.hlsl", shader_byte_code, size_shader));
	m_weather_atmosphere->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_simple_stage = std::shared_ptr<Shader>(new Shader(L"SimpleStageVS.hlsl", L"SimpleStagePS.hlsl", shader_byte_code, size_shader));
	m_simple_stage->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_triplanar_texture = std::shared_ptr<Shader>(new Shader(L"TriplanarTextureVS.hlsl", L"TriplanarTexturePS.hlsl", shader_byte_code, size_shader));
	m_triplanar_texture->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_toon_model = std::shared_ptr<Shader>(new Shader(L"ToonVS.hlsl", L"ToonPS.hlsl", shader_byte_code, size_shader));
	m_toon_model->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//
	
	m_toon_tex_model = std::shared_ptr<Shader>(new Shader(L"ToonTexVS.hlsl", L"ToonTexPS.hlsl", shader_byte_code, size_shader));
	m_toon_tex_model->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_gradient = std::shared_ptr<Shader>(new Shader(L"TriplanarTextureVS.hlsl", L"TriplanarTexturePS.hlsl", shader_byte_code, size_shader));
	m_gradient->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	m_hatch = std::shared_ptr<Shader>(new Shader(L"TriplanarTextureVS.hlsl", L"TriplanarTexturePS.hlsl", shader_byte_code, size_shader));
	m_hatch->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
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
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"FinalPassBasicVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_FinalBasicVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"FinalPassBasicPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_FinalBasicPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_final_basic = std::shared_ptr<Shader>(new Shader(L"FinalPassBasicVS.hlsl", L"FinalPassBasicPS.hlsl", shader_byte_code, size_shader));
	m_final_basic->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

}

void ShaderManager::recompileErrorShaders()
{
	m_flat->ifErrorRecompile(m_error);
	m_flat_tex->ifErrorRecompile(m_error);
	m_lambert->ifErrorRecompile(m_error);
	m_lambert_specular->ifErrorRecompile(m_error);
	m_lambert_specular_rimlight->ifErrorRecompile(m_error);
	m_texture->ifErrorRecompile(m_error);
	m_texture_normal->ifErrorRecompile(m_error);
	m_texture_normal_gloss->ifErrorRecompile(m_error);
	m_environment_tex->ifErrorRecompile(m_error);
	m_geo_test->ifErrorRecompile(m_error);
	m_tess_model->ifErrorRecompile(m_error);
	m_tess_terrain->ifErrorRecompile(m_error);
	m_deferred->ifErrorRecompile(m_error);
	m_shadowmap->ifErrorRecompile(m_error);
	m_shadowmap_terrain->ifErrorRecompile(m_error);
	m_tess_terrain_splat->ifErrorRecompile(m_error);
	m_tess_terrain_splat_demo->ifErrorRecompile(m_error);
	m_terrain_test->ifErrorRecompile(m_error);
	m_terrain_ld->ifErrorRecompile(m_error);
	m_terrain_md->ifErrorRecompile(m_error);
	//m_terrain_hd->ifErrorRecompile();
	m_terrain_ld_toon->ifErrorRecompile(m_error);
	m_terrain_md_toon->ifErrorRecompile(m_error);
	m_terrain_hd_toon->ifErrorRecompile(m_error);
	m_atmosphere->ifErrorRecompile(m_error);
	m_white_noise->ifErrorRecompile(m_error);
	m_value_noise->ifErrorRecompile(m_error);
	m_perlin_noise->ifErrorRecompile(m_error);
	m_dynamic_noise->ifErrorRecompile(m_error);
	m_perlin_voronoi_noise->ifErrorRecompile(m_error);
	m_volume_cloud->ifErrorRecompile(m_error);
	m_weather_map->ifErrorRecompile(m_error);
	m_weather_atmosphere->ifErrorRecompile(m_error);
	m_3D_tex->ifErrorRecompile(m_error);
	m_simple_stage->ifErrorRecompile(m_error);
	m_triplanar_texture->ifErrorRecompile(m_error);
	m_toon_model->ifErrorRecompile(m_error);
	m_toon_tex_model->ifErrorRecompile(m_error);
}

void ShaderManager::recompileShader(int shader)
{
	switch (shader)
	{
	case Shaders::FLAT:							m_flat->recompile(m_error); break;
	case Shaders::FLAT_TEX:						m_flat_tex->recompile(m_error); break;
	case Shaders::LAMBERT:						m_lambert->recompile(m_error); break;
	case Shaders::LAMBERT_SPECULAR:				m_lambert_specular->recompile(m_error); break;
	case Shaders::LAMBERT_RIMLIGHT:				m_lambert_specular_rimlight->recompile(m_error); break;
	case Shaders::TEXTURE:						m_texture->recompile(m_error); break;
	case Shaders::TEXTURE_NORMAL:				m_texture_normal->recompile(m_error); break;
	case Shaders::TEXTURE_NORMAL_GLOSS:			m_texture_normal_gloss->recompile(m_error); break;
	case Shaders::TEXTURE_ENVIRONMENT:			m_environment_tex->recompile(m_error); break;
	case Shaders::GEO_TEST:						m_geo_test->recompile(m_error); break;
	case Shaders::TEXTURE_TESS_MODEL:			m_tess_model->recompile(m_error); break;
	case Shaders::TEXTURE_TESS_TERRAIN:			m_tess_terrain->recompile(m_error); break;
	//case Shaders::TEXTURE_TESS_FLUID_TERRAIN:	m_flat->recompile(); break;
	case Shaders::DEFERRED1:					m_deferred->recompile(m_error); break;
	case Shaders::SHADOWMAP:					m_shadowmap->recompile(m_error); break;
	case Shaders::SHADOWMAP_TERRAIN:			m_shadowmap_terrain->recompile(m_error); break;
	case Shaders::TEXTURE_TESS_3SPLAT:			m_tess_terrain_splat->recompile(m_error); break;
	//case Shaders::TEXTURE_TESS_3SPLAT_GRASS:	m_flat->recompile(); break;
	case Shaders::TERRAIN_TEST:					m_terrain_test->recompile(m_error); break;
	case Shaders::TERRAIN_LD:					m_terrain_ld->recompile(m_error); break;
	case Shaders::TERRAIN_MD:					m_terrain_md->recompile(m_error); break;
	case Shaders::TERRAIN_LD_TOON:				m_terrain_ld_toon->recompile(m_error); break;
	case Shaders::TERRAIN_MD_TOON:				m_terrain_md_toon->recompile(m_error); break;
	case Shaders::TERRAIN_HD_TOON:				m_terrain_hd_toon->recompile(m_error); break;
	case Shaders::ATMOSPHERE:					m_atmosphere->recompile(m_error); break;
	case Shaders::TESSDEMO:						m_tess_terrain_splat_demo->recompile(m_error); break;
	case Shaders::WHITE_NOISE:					m_white_noise->recompile(m_error); break;
	case Shaders::VALUE_NOISE:					m_value_noise->recompile(m_error); break;
	case Shaders::PERLIN_NOISE:					m_perlin_noise->recompile(m_error); break;
	case Shaders::DYNAMIC_NOISE:				m_dynamic_noise->recompile(m_error); break;
	case Shaders::PERLIN_VORONOI_NOISE:			m_perlin_voronoi_noise->recompile(m_error); break;
	case Shaders::VOLUME_CLOUD:					m_volume_cloud->recompile(m_error); break;
	case Shaders::_3DTEX:						m_3D_tex->recompile(m_error); break;
	case Shaders::WEATHER_MAP:					m_weather_map->recompile(m_error); break;
	case Shaders::WEATHER_ATMOSPHERE:			m_weather_atmosphere->recompile(m_error); break;
	case Shaders::SIMPLE_STAGE:					m_simple_stage->recompile(m_error); break;
	case Shaders::TRIPLANAR_TEXTURE:			m_triplanar_texture->recompile(m_error); break;
	case Shaders::TOON_MODEL:					m_toon_model->recompile(m_error); break;
	case Shaders::TOON_TEX_MODEL:				m_toon_tex_model->recompile(m_error); break;
	}
}

void ShaderManager::imGuiMenuShaderCompile()
{
	if (ImGui::BeginMenu("Recompile"))
	{
		ImVec2 size = ImVec2(200, 20);
		if (ImGui::Button("Flat", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::FLAT);
		if (ImGui::Button("Flat Tex", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::FLAT_TEX);
		if (ImGui::Button("Lambert", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::LAMBERT);
		if (ImGui::Button("Lambert Specular", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::LAMBERT_SPECULAR);
		if (ImGui::Button("Lambert Rimlight", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::LAMBERT_RIMLIGHT);
		if (ImGui::Button("Texture", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE);
		if (ImGui::Button("Texture Normal", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_NORMAL);
		if (ImGui::Button("Texture Normal Gloss", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_NORMAL_GLOSS);
		if (ImGui::Button("Texture Environment", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_ENVIRONMENT);
		if (ImGui::Button("Geo Test", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::GEO_TEST);
		if (ImGui::Button("Tesselation Model", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_TESS_MODEL);
		if (ImGui::Button("Tesselation Terrain", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_TESS_TERRAIN);
		if (ImGui::Button("Deferred1", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::DEFERRED1);
		if (ImGui::Button("Shadowmap Model", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::SHADOWMAP);
		if (ImGui::Button("Shadowmap Terrain", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::SHADOWMAP_TERRAIN);
		if (ImGui::Button("Terrain Test", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_TEST);
		if (ImGui::Button("Terrain LD", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_LD);
		if (ImGui::Button("Terrain MD", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_MD);
		if (ImGui::Button("Terrain HD", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_TESS_3SPLAT);
		if (ImGui::Button("Terrain LD Toon", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_LD_TOON);
		if (ImGui::Button("Terrain MD Toon", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_MD_TOON);
		if (ImGui::Button("Terrain HD Toon", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_HD_TOON);
		if (ImGui::Button("Atmosphere", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::ATMOSPHERE);
		if (ImGui::Button("Terrain Tesselation Demo", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TESSDEMO);
		if (ImGui::Button("White Noise", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::WHITE_NOISE);
		if (ImGui::Button("Value Noise", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::VALUE_NOISE);
		if (ImGui::Button("Perlin Noise", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::PERLIN_NOISE);
		if (ImGui::Button("Dynamic Noise", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::DYNAMIC_NOISE);
		if (ImGui::Button("Perlin Voronoi Noise", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::PERLIN_VORONOI_NOISE);
		if (ImGui::Button("Volume Cloud", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::VOLUME_CLOUD);
		if (ImGui::Button("3D Texture", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::_3DTEX);
		if (ImGui::Button("Weather Map", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::WEATHER_MAP);
		if (ImGui::Button("Weather Atmosphere", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::WEATHER_ATMOSPHERE);
		if (ImGui::Button("Simple Stage", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::SIMPLE_STAGE);
		if (ImGui::Button("Triplanar Texture", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TRIPLANAR_TEXTURE);
		if (ImGui::Button("Toon Model", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TOON_MODEL);
		if (ImGui::Button("Toon Model Textured", size)) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TOON_TEX_MODEL);
		ImGui::EndMenu();
	}
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
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_final_basic->m_vs);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_final_basic->m_ps);
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
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_flat->m_ps);
		break;
	case FLAT_TEX_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_flat_tex->m_ps);
		break;
	case LAMBERT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_lambert->m_ps);
		break;
	case LAMBERT_SPECULAR_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_lambert_specular->m_ps);
		break;
	case LAMBERT_RIMLIGHT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_lambert_specular_rimlight->m_ps);
		break;
	case TEXTURE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_texture->m_ps);
		break;
	case TEXTURE_NORMAL_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_texture_normal->m_ps);
		break;
	case TEXTURE_NORMAL_GLOSS_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_texture_normal_gloss->m_ps);
		break;
	case TEXTURE_ENVIRONMENT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_environment_tex->m_ps);
		break;
	case GEO_TEST_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_geo_test->m_ps);
		break;
	case TEXTURE_TESS_MODEL_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_tess_model->m_ps);
		break;
	case TEXTURE_TESS_TERRAIN_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_tess_terrain->m_ps);
		break;
	case DEFERRED1_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_deferred->m_ps);
		break;
	case SHADOWMAP_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_shadowmap->m_ps);
		break;
	case TEXTURE_TESS_3SPLAT_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_tess_terrain_splat->m_ps);
		break;
	case TERRAIN_TEST_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_terrain_test->m_ps);
		break;
	case TERRAIN_LD_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_terrain_ld->m_ps);
		break;
	case TERRAIN_MD_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_terrain_md->m_ps);
		break;
	case TERRAIN_MD_TOON_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_terrain_md_toon->m_ps);
		break;
	case TERRAIN_HD_TOON_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_terrain_hd_toon->m_ps);
		break;
	case ATMOSPHERE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_atmosphere->m_ps);
		break;
	case WHITE_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_white_noise->m_ps);
		break;
	case VALUE_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_value_noise->m_ps);
		break;
	case PERLIN_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_perlin_noise->m_ps);
		break;
	case DYNAMIC_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_dynamic_noise->m_ps);
		break;
	case PERLIN_VORONOI_NOISE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_perlin_voronoi_noise->m_ps);
		break;
	case VOLUME_CLOUD_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_volume_cloud->m_ps);
		break;
	case _3DTEX_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_3D_tex->m_ps);
		break;
	case WEATHER_MAP_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_weather_map->m_ps);
		break;
	case WEATHER_ATMOSPHERE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_weather_atmosphere->m_ps);
		break;
	case SIMPLE_STAGE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_simple_stage->m_ps);
		break;
	case TRIPLANAR_TEXTURE_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_triplanar_texture->m_ps);
		break;
	case TOON_MODEL_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_toon_model->m_ps);
		break;
	case TOON_MODEL_TEX_PS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_toon_tex_model->m_ps);
		break;
	}
}

void ShaderManager::setVertexShader(int type)
{
	switch (type)
	{
	case FLAT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_flat->m_vs);
		break;
	case LAMBERT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_lambert->m_vs);
		break;
	case LAMBERT_SPECULAR_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_lambert_specular->m_vs);
		break;
	case LAMBERT_RIMLIGHT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_lambert_specular_rimlight->m_vs);
		break;
	case TEXTURE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_texture->m_vs);
		break;
	case TEXTURE_NORMAL_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_texture_normal->m_vs);
		break;
	case TEXTURE_NORMAL_GLOSS_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_texture_normal_gloss->m_vs);
		break;
	case TEXTURE_ENVIRONMENT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_environment_tex->m_vs);
		break;
	case GEO_TEST_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_geo_test->m_vs);
		break;
	case TEXTURE_TESS_MODEL_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_tess_model->m_vs);
		break;
	case TEXTURE_TESS_TERRAIN_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_tess_terrain->m_vs);
		break;
	case DEFERRED1_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_deferred->m_vs);
		break;
	case SHADOWMAP_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_shadowmap->m_vs);
		break;
	case SHADOWMAP_TERRAIN_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_shadowmap_terrain->m_vs);
		break;
	case TEXTURE_TESS_3SPLAT_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_tess_terrain_splat->m_vs);
		break;
	case TERRAIN_TEST_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_terrain_test->m_vs);
		break;
	case TERRAIN_LD_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_terrain_ld->m_vs);
		break;
	case TERRAIN_MD_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_terrain_md->m_vs);
		break;
	case TERRAIN_LD_TOON_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_terrain_ld_toon->m_vs);
		break;
	case ATMOSPHERE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_atmosphere->m_vs);
		break;
	case TEXTURE_TESS_3SPLAT_DEMO_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_tess_terrain_splat_demo->m_vs);
		break;
	case WHITE_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_white_noise->m_vs);
		break;
	case VALUE_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_value_noise->m_vs);
		break;
	case PERLIN_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_perlin_noise->m_vs);
		break;
	case DYNAMIC_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_dynamic_noise->m_vs);
		break;
	case PERLIN_VORONOI_NOISE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_perlin_voronoi_noise->m_vs);
		break;
	case VOLUME_CLOUD_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_volume_cloud->m_vs);
		break;
	case _3DTEX_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_3D_tex->m_vs);
		break;
	case WEATHER_MAP_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_weather_map->m_vs);
		break;
	case WEATHER_ATMOSPHERE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_weather_atmosphere->m_vs);
		break;
	case SIMPLE_STAGE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_simple_stage->m_vs);
		break;
	case TRIPLANAR_TEXTURE_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_triplanar_texture->m_vs);
		break;
	case TOON_MODEL_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_toon_model->m_vs);
		break;
	case TOON_MODEL_TEX_VS:
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_toon_tex_model->m_vs);
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