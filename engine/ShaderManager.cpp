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

	//error shader - used when a shader is unable to compile
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"ErrorVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_ErrorVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"ErrorPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_ErrorPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_error = std::shared_ptr<Shader>(new Shader(L"ErrorVS.hlsl", L"ErrorPS.hlsl", shader_byte_code, size_shader));
	//***************************************************************************//

	////flat shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"FlatShaderVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_FlatVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"FlatShaderPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_FlatPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_flat = std::shared_ptr<Shader>(new Shader(L"FlatShaderVS.hlsl", L"FlatShaderPS.hlsl", shader_byte_code, size_shader));
	m_flat->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_flat_tex = std::shared_ptr<Shader>(new Shader(L"FlatShaderVS.hlsl", L"FlatTexShaderPS.hlsl", shader_byte_code, size_shader));
	m_flat_tex->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//
	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"FlatTexShaderPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_FlatTexPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//***************************************************************************//

	////lambert shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"LambertVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_LambertVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"LambertPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_LambertPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_lambert = std::shared_ptr<Shader>(new Shader(L"LambertVS.hlsl", L"LambertPS.hlsl", shader_byte_code, size_shader));
	m_lambert->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//specular shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"SpecularVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_Lambert_SpecularVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SpecularPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_Lambert_SpecularPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_lambert_specular = std::shared_ptr<Shader>(new Shader(L"SpecularVS.hlsl", L"SpecularPS.hlsl", shader_byte_code, size_shader));
	m_lambert_specular->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//rimlight shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"RimlightVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_Lambert_Specular_RimlightVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"RimlightPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_Lambert_Specular_RimlightPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_lambert_specular_rimlight = std::shared_ptr<Shader>(new Shader(L"RimlightVS.hlsl", L"RimlightPS.hlsl", shader_byte_code, size_shader));
	m_lambert_specular_rimlight->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//textured rimlight shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TexturedRimlightVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TextureVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TexturedRimlightPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TexturePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_texture = std::shared_ptr<Shader>(new Shader(L"TexturedRimlightVS.hlsl", L"TexturedRimlightPS.hlsl", shader_byte_code, size_shader));
	m_texture->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//textured normal map shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TexturedNormalMapVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TextureNormalVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TexturedNormalMapPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TextureNormalPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_texture_normal = std::shared_ptr<Shader>(new Shader(L"TexturedNormalMapVS.hlsl", L"TexturedNormalMapPS.hlsl", shader_byte_code, size_shader));
	m_texture_normal->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//textured normal map shader with specular map
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"GlossVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TextureNormalGlossVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"GlossPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TextureNormalGlossPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_texture_normal_gloss = std::shared_ptr<Shader>(new Shader(L"GlossVS.hlsl", L"GlossPS.hlsl", shader_byte_code, size_shader));
	m_texture_normal_gloss->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//textured shader with environmental reflection texture map
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"EnvironmentTexMapVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_EnvTexVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"EnvironmentTexMapPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_EnvTexPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_environment_tex = std::shared_ptr<Shader>(new Shader(L"EnvironmentTexMapVS.hlsl", L"EnvironmentTexMapPS.hlsl", shader_byte_code, size_shader));
	m_environment_tex->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//copy of textured rimlight vs to test geometry shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"GeometryTestVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_GeoTestVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"GeometryTestPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_GeoTestPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_geo_test = std::shared_ptr<Shader>(new Shader(L"GeometryTestVS.hlsl", L"GeometryTestPS.hlsl", shader_byte_code, size_shader));
	m_geo_test->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//textured shader which tesselates and uses a displacement map for dynamic detail LOD for models
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TessModelVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TessModelVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TessModelPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TessModelPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_tess_model = std::shared_ptr<Shader>(new Shader(L"GeometryTestVS.hlsl", L"GeometryTestPS.hlsl", shader_byte_code, size_shader));
	m_tess_model->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//textured shader which tesselates and uses a displacement map for dynamic detail LOD modified for terrain
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TessTerrainVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TessTerrainVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TessFluidTerrainVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TessFluidTerrainVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TessTerrainPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TessTerrainPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_tess_terrain = std::shared_ptr<Shader>(new Shader(L"TessTerrainVS.hlsl", L"TessTerrainPS.hlsl", shader_byte_code, size_shader));
	m_tess_terrain->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	//***************************************************************************//

	//sample shaders for deferred rendering
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"DeferredVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_DeferredVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"DeferredPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_DeferredPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_deferred = std::shared_ptr<Shader>(new Shader(L"DeferredVS.hlsl", L"DeferredPS.hlsl", shader_byte_code, size_shader));
	m_deferred->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//shader for directional light shadowmapping
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"DirLightShadowMapVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_ShadowMapVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"DirLightShadowMapTessVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_ShadowMapTerrainVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"DirLightShadowMapPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_ShadowMapPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_shadowmap = std::shared_ptr<Shader>(new Shader(L"DirLightShadowMapVS.hlsl", L"DirLightShadowMapPS.hlsl", shader_byte_code, size_shader));
	m_shadowmap->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_shadowmap_terrain = std::shared_ptr<Shader>(new Shader(L"DirLightShadowMapTessVS.hlsl", L"DirLightShadowMapPS.hlsl", shader_byte_code, size_shader));
	m_shadowmap_terrain->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//shader for texture splat terrain
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"Terrain3SplatVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TessTerrain3SplatVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"Terrain3SplatDemoVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TessTerrain3SplatDemoVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"Terrain3SplatPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TessTerrain3SplatPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_tess_terrain_splat = std::shared_ptr<Shader>(new Shader(L"Terrain3SplatVS.hlsl", L"Terrain3SplatPS.hlsl", shader_byte_code, size_shader));
	m_tess_terrain_splat->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_tess_terrain_splat_demo = std::shared_ptr<Shader>(new Shader(L"Terrain3SplatDemoVS.hlsl", L"Terrain3SplatPS.hlsl", shader_byte_code, size_shader));
	m_tess_terrain_splat_demo->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	//***************************************************************************//

	//shader for troubleshooting terrain
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TerrainTestVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TerrainTestVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TerrainTestPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TerrainTestPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_terrain_test = std::shared_ptr<Shader>(new Shader(L"TerrainTestVS.hlsl", L"TerrainTestPS.hlsl", shader_byte_code, size_shader));
	m_terrain_test->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//shader for low definition terrain
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"LDTerrainVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TerrainLDVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"LDTerrainPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TerrainLDPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_terrain_ld = std::shared_ptr<Shader>(new Shader(L"LDTerrainVS.hlsl", L"LDTerrainPS.hlsl", shader_byte_code, size_shader));
	m_terrain_ld->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//shader for mid definition terrain
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"MDTerrainVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TerrainMDVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"MDTerrainPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TerrainMDPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_terrain_md = std::shared_ptr<Shader>(new Shader(L"MDTerrainVS.hlsl", L"MDTerrainPS.hlsl", shader_byte_code, size_shader));
	m_terrain_md->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//shaders for HD toon terrain
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"LDTerrainVSToon.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_TerrainLDVS_toon = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"MDTerrainPSToon.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TerrainMDPS_toon = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"HDTerrainPSToon.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_TerrainHDPS_toon = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_terrain_ld_toon = std::shared_ptr<Shader>(new Shader(L"LDTerrainVSToon.hlsl", L"LDTerrainPS.hlsl", shader_byte_code, size_shader));
	m_terrain_ld_toon->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_terrain_md_toon = std::shared_ptr<Shader>(new Shader(L"MDTerrainVS.hlsl", L"MDTerrainPSToon.hlsl", shader_byte_code, size_shader));
	m_terrain_md_toon->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);

	m_terrain_hd_toon = std::shared_ptr<Shader>(new Shader(L"Terrain3SplatVS.hlsl", L"HDTerrainPSToon.hlsl", shader_byte_code, size_shader));
	m_terrain_hd_toon->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//shader for dynamic skybox
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"AtmosphericShaderVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_AtmosphereVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"AtmosphericShaderPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_AtmospherePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_atmosphere = std::shared_ptr<Shader>(new Shader(L"AtmosphericShaderVS.hlsl", L"AtmosphericShaderPS.hlsl", shader_byte_code, size_shader));
	m_atmosphere->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//white noise shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"WhiteNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_WhiteNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"WhiteNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_WhiteNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_white_noise = std::shared_ptr<Shader>(new Shader(L"WhiteNoiseVS.hlsl", L"WhiteNoisePS.hlsl", shader_byte_code, size_shader));
	m_white_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//value noise shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"ValueNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_ValueNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"ValueNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_ValueNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_value_noise = std::shared_ptr<Shader>(new Shader(L"ValueNoiseVS.hlsl", L"ValueNoisePS.hlsl", shader_byte_code, size_shader));
	m_value_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//perlin noise shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"PerlinNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_PerlinNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PerlinNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_PerlinNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_perlin_noise = std::shared_ptr<Shader>(new Shader(L"PerlinNoiseVS.hlsl", L"PerlinNoisePS.hlsl", shader_byte_code, size_shader));
	m_perlin_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//perlin voronoi noise shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"PerlinVoronoiNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_PerlinVoronoiNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PerlinVoronoiNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_PerlinVoronoiNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_perlin_voronoi_noise = std::shared_ptr<Shader>(new Shader(L"PerlinVoronoiNoiseVS.hlsl", L"PerlinVoronoiNoisePS.hlsl", shader_byte_code, size_shader));
	m_perlin_voronoi_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//dynamic noise shader for runtime adjustments before exporting texture data
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"DynamicNoiseVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_DynamicNoiseVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"DynamicNoisePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_DynamicNoisePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_dynamic_noise = std::shared_ptr<Shader>(new Shader(L"DynamicNoiseVS.hlsl", L"DynamicNoisePS.hlsl", shader_byte_code, size_shader));
	m_dynamic_noise->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//volumetric cloud shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"CloudVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_VolumeCloudVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"CloudPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_VolumeCloudPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_volume_cloud = std::shared_ptr<Shader>(new Shader(L"CloudVS.hlsl", L"CloudPS.hlsl", shader_byte_code, size_shader));
	m_volume_cloud->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//3D Texture confirmation shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"3DTexTestVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_3DTexVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"3DTexTestPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_3DTexPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_3D_tex = std::shared_ptr<Shader>(new Shader(L"3DTexTestVS.hlsl", L"3DTexTestPS.hlsl", shader_byte_code, size_shader));
	m_3D_tex->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//Shader for generating weather maps used for cloudscapes
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"WeatherMapVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_WeatherMapVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"WeatherMapPS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_WeatherMapPS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_weather_map = std::shared_ptr<Shader>(new Shader(L"WeatherMapVS.hlsl", L"WeatherMapPS.hlsl", shader_byte_code, size_shader));
	m_weather_map->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//Atmosphere shader with added clouds based on the weathermap shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"WeatherAtmosphereVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_WeatherAtmosphereVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"WeatherAtmospherePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_WeatherAtmospherePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_weather_atmosphere = std::shared_ptr<Shader>(new Shader(L"WeatherAtmosphereVS.hlsl", L"WeatherAtmospherePS.hlsl", shader_byte_code, size_shader));
	m_weather_atmosphere->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
	//***************************************************************************//

	//Atmosphere shader with added clouds based on the weathermap shader
	//GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"SimpleStageVS.hlsl", "vsmain", &shader_byte_code, &size_shader);
	//m_SimpleStageVS = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	//GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SimpleStagePS.hlsl", "psmain", &shader_byte_code, &size_shader);
	//m_SimpleStagePS = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_simple_stage = std::shared_ptr<Shader>(new Shader(L"SimpleStageVS.hlsl", L"SimpleStagePS.hlsl", shader_byte_code, size_shader));
	m_simple_stage->ifErrorReplaceShaders(m_error->m_vs, m_error->m_ps);
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
	m_flat->ifErrorRecompile();
	m_flat_tex->ifErrorRecompile();
	m_lambert->ifErrorRecompile();
	m_lambert_specular->ifErrorRecompile();
	m_lambert_specular_rimlight->ifErrorRecompile();
	m_texture->ifErrorRecompile();
	m_texture_normal->ifErrorRecompile();
	m_texture_normal_gloss->ifErrorRecompile();
	m_environment_tex->ifErrorRecompile();
	m_geo_test->ifErrorRecompile();
	m_tess_model->ifErrorRecompile();
	m_tess_terrain->ifErrorRecompile();
	m_deferred->ifErrorRecompile();
	m_shadowmap->ifErrorRecompile();
	m_shadowmap_terrain->ifErrorRecompile();
	m_tess_terrain_splat->ifErrorRecompile();
	m_tess_terrain_splat_demo->ifErrorRecompile();
	m_terrain_test->ifErrorRecompile();
	m_terrain_ld->ifErrorRecompile();
	m_terrain_md->ifErrorRecompile();
	//m_terrain_hd->ifErrorRecompile();
	m_terrain_ld_toon->ifErrorRecompile();
	m_terrain_md_toon->ifErrorRecompile();
	m_terrain_hd_toon->ifErrorRecompile();
	m_atmosphere->ifErrorRecompile();
	m_white_noise->ifErrorRecompile();
	m_value_noise->ifErrorRecompile();
	m_perlin_noise->ifErrorRecompile();
	m_dynamic_noise->ifErrorRecompile();
	m_perlin_voronoi_noise->ifErrorRecompile();
	m_volume_cloud->ifErrorRecompile();
	m_weather_map->ifErrorRecompile();
	m_weather_atmosphere->ifErrorRecompile();
	m_3D_tex->ifErrorRecompile();
	m_simple_stage->ifErrorRecompile();
}

void ShaderManager::recompileShader(int shader)
{
	switch (shader)
	{
	case Shaders::FLAT:							m_flat->recompile(); break;
	case Shaders::FLAT_TEX:						m_flat_tex->recompile(); break;
	case Shaders::LAMBERT:						m_lambert->recompile(); break;
	case Shaders::LAMBERT_SPECULAR:				m_lambert_specular->recompile(); break;
	case Shaders::LAMBERT_RIMLIGHT:				m_lambert_specular_rimlight->recompile(); break;
	case Shaders::TEXTURE:						m_texture->recompile(); break;
	case Shaders::TEXTURE_NORMAL:				m_texture_normal->recompile(); break;
	case Shaders::TEXTURE_NORMAL_GLOSS:			m_texture_normal_gloss->recompile(); break;
	case Shaders::TEXTURE_ENVIRONMENT:			m_environment_tex->recompile(); break;
	case Shaders::GEO_TEST:						m_geo_test->recompile(); break;
	case Shaders::TEXTURE_TESS_MODEL:			m_tess_model->recompile(); break;
	case Shaders::TEXTURE_TESS_TERRAIN:			m_tess_terrain->recompile(); break;
	//case Shaders::TEXTURE_TESS_FLUID_TERRAIN:	m_flat->recompile(); break;
	case Shaders::DEFERRED1:					m_deferred->recompile(); break;
	case Shaders::SHADOWMAP:					m_shadowmap->recompile(); break;
	case Shaders::SHADOWMAP_TERRAIN:			m_shadowmap_terrain->recompile(); break;
	case Shaders::TEXTURE_TESS_3SPLAT:			m_tess_terrain_splat->recompile(); break;
	//case Shaders::TEXTURE_TESS_3SPLAT_GRASS:	m_flat->recompile(); break;
	case Shaders::TERRAIN_TEST:					m_terrain_test->recompile(); break;
	case Shaders::TERRAIN_LD:					m_terrain_ld->recompile(); break;
	case Shaders::TERRAIN_MD:					m_terrain_md->recompile(); break;
	case Shaders::TERRAIN_LD_TOON:				m_terrain_ld_toon->recompile(); break;
	case Shaders::TERRAIN_MD_TOON:				m_terrain_md_toon->recompile(); break;
	case Shaders::TERRAIN_HD_TOON:				m_terrain_hd_toon->recompile(); break;
	case Shaders::ATMOSPHERE:					m_atmosphere->recompile(); break;
	case Shaders::TESSDEMO:						m_tess_terrain_splat_demo->recompile(); break;
	case Shaders::WHITE_NOISE:					m_white_noise->recompile(); break;
	case Shaders::VALUE_NOISE:					m_value_noise->recompile(); break;
	case Shaders::PERLIN_NOISE:					m_perlin_noise->recompile(); break;
	case Shaders::DYNAMIC_NOISE:				m_dynamic_noise->recompile(); break;
	case Shaders::PERLIN_VORONOI_NOISE:			m_perlin_voronoi_noise->recompile(); break;
	case Shaders::VOLUME_CLOUD:					m_volume_cloud->recompile(); break;
	case Shaders::_3DTEX:						m_3D_tex->recompile(); break;
	case Shaders::WEATHER_MAP:					m_weather_map->recompile(); break;
	case Shaders::WEATHER_ATMOSPHERE:			m_weather_atmosphere->recompile(); break;
	case Shaders::SIMPLE_STAGE:					m_simple_stage->recompile(); break;
	}
}

void ShaderManager::imGuiMenuShaderCompile()
{
	if (ImGui::BeginMenu("Recompile Shader"))
	{
		if (ImGui::Button("Flat")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::FLAT);
		if (ImGui::Button("Flat Tex")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::FLAT_TEX);
		if (ImGui::Button("Lambert")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::LAMBERT);
		if (ImGui::Button("Lambert Specular")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::LAMBERT_SPECULAR);
		if (ImGui::Button("Lambert Rimlight")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::LAMBERT_RIMLIGHT);
		if (ImGui::Button("Texture")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE);
		if (ImGui::Button("Texture Normal")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_NORMAL);
		if (ImGui::Button("Texture Normal Gloss")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_NORMAL_GLOSS);
		if (ImGui::Button("Texture Environment")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_ENVIRONMENT);
		if (ImGui::Button("Geo Test")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::GEO_TEST);
		if (ImGui::Button("Tesselation Model")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_TESS_MODEL);
		if (ImGui::Button("Tesselation Terrain")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_TESS_TERRAIN);
		if (ImGui::Button("Deferred1")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::DEFERRED1);
		if (ImGui::Button("Shadowmap Model")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::SHADOWMAP);
		if (ImGui::Button("Shadowmap Terrain")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::SHADOWMAP_TERRAIN);
		if (ImGui::Button("Terrain Test")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_TEST);
		if (ImGui::Button("Terrain LD")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_LD);
		if (ImGui::Button("Terrain MD")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_MD);
		if (ImGui::Button("Terrain HD")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TEXTURE_TESS_3SPLAT);
		if (ImGui::Button("Terrain LD Toon")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_LD_TOON);
		if (ImGui::Button("Terrain MD Toon")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_MD_TOON);
		if (ImGui::Button("Terrain HD Toon")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TERRAIN_HD_TOON);
		if (ImGui::Button("Atmosphere")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::ATMOSPHERE);
		if (ImGui::Button("Terrain Tesselation Demo")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::TESSDEMO);
		if (ImGui::Button("White Noise")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::WHITE_NOISE);
		if (ImGui::Button("Value Noise")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::VALUE_NOISE);
		if (ImGui::Button("Perlin Noise")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::PERLIN_NOISE);
		if (ImGui::Button("Dynamic Noise")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::DYNAMIC_NOISE);
		if (ImGui::Button("Perlin Voronoi Noise")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::PERLIN_VORONOI_NOISE);
		if (ImGui::Button("Volume Cloud")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::VOLUME_CLOUD);
		if (ImGui::Button("3D Texture")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::_3DTEX);
		if (ImGui::Button("Weather Map")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::WEATHER_MAP);
		if (ImGui::Button("Weather Atmosphere")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::WEATHER_ATMOSPHERE);
		if (ImGui::Button("Simple Stage")) GraphicsEngine::get()->getShaderManager()->recompileShader(Shaders::SIMPLE_STAGE);
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
	//case TEXTURE_TESS_FLUID_TERRAIN_VS:
	//	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_TessFluidTerrainVS);
	//	break;
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