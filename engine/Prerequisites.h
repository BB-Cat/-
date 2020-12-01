#pragma once
#include <memory>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

class SwapChain;
class DeviceContext;
class GBuffer;
class VertexBuffer;
class IndexBuffer;
class VertexShader;
class GeometryShader;
class HullShader;
class DomainShader;
class PixelShader;
class ComputeShader;
class Sampler;
class MyConstantBuffer;
class MyGeometricPrimitive;
class RenderSystem;
class Shader;
class Resource;
class ResourceManager;
class Texture;
class Texture3D;
class TextureManager;
class Sprite;
class SpriteManager;
class StaticMesh;
class Subset_Obj;
class Material_Obj;
class Meshmanager;
class SkinnedMesh;
class ConstantBufferSystem;
class BlendMode;
class GraphicsEngine;

//primitive classes
class Cube;

//game classes
class Player;
class Enemy;
class PowerUp;
class Terrain;
class TerrainManager;
//

typedef std::shared_ptr<SwapChain> SwapChainPtr;
typedef std::shared_ptr<DeviceContext> DeviceContextPtr;
typedef std::shared_ptr<GBuffer> GBufferPtr;
typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
typedef std::shared_ptr<MyConstantBuffer> MyConstantBufferPtr;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;
typedef std::shared_ptr<GeometryShader> GeometryShaderPtr;
typedef std::shared_ptr<HullShader> HullShaderPtr;
typedef std::shared_ptr<DomainShader> DomainShaderPtr;
typedef std::shared_ptr<PixelShader> PixelShaderPtr;
typedef std::shared_ptr<ComputeShader> ComputeShaderPtr;
typedef std::shared_ptr<Sampler> SamplerPtr;
typedef std::shared_ptr<Resource> ResourcePtr;
typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<Texture3D> Texture3DPtr;
typedef std::shared_ptr<Sprite> SpritePtr;
typedef std::shared_ptr<StaticMesh> StaticMeshPtr;
typedef std::shared_ptr<SkinnedMesh> SkinnedMeshPtr;
typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::shared_ptr<Cube> CubePtr;

//game type definitions
typedef std::shared_ptr<Player> PlayerPtr;
typedef std::shared_ptr<Enemy> EnemyPtr;
typedef std::shared_ptr<PowerUp> PowerUpPtr;
typedef std::shared_ptr<Terrain> TerrainPtr;
//

//pixel shader manager enumeration for setting shaders in the pipeline
enum PS
{
	ONE_TEX,
	NO_TEX,
	NO_TEX_TERRAIN,
	SPRITE,
	FLAT_PS,
	FLAT_TEX_PS,
	LAMBERT_PS,
	LAMBERT_SPECULAR_PS,
	LAMBERT_RIMLIGHT_PS,
	TEXTURE_PS,
	TEXTURE_NORMAL_PS,
	TEXTURE_NORMAL_GLOSS_PS,
	TEXTURE_ENVIRONMENT_PS,
	GEO_TEST_PS,
	TEXTURE_TESS_MODEL_PS,
	TEXTURE_TESS_TERRAIN_PS,
	GRASS_PS,
	DEFERRED1_PS,
	SHADOWMAP_PS,
	TEXTURE_TESS_3SPLAT_PS,
	TERRAIN_TEST_PS,
	TERRAIN_LD_PS,
	TERRAIN_MD_PS,
	TERRAIN_MD_TOON_PS,
	TERRAIN_HD_TOON_PS,
	ATMOSPHERE_PS,
	WHITE_NOISE_PS,
	VALUE_NOISE_PS,
	PERLIN_NOISE_PS,
	DYNAMIC_NOISE_PS,
	PERLIN_VORONOI_NOISE_PS,
	VOLUME_CLOUD_PS,
	_3DTEX_PS,
	WEATHER_MAP_PS,
	WEATHER_ATMOSPHERE_PS,
	SIMPLE_STAGE_PS,
	TRIPLANAR_TEXTURE_PS,
	TOON_MODEL_PS,
	TOON_MODEL_TEX_PS,
};
//vertex shader enumeration for setting shaders in the pipeline
enum VS
{
	MODEL,
	TERRAIN,
	FLAT_VS,
	LAMBERT_VS,
	LAMBERT_SPECULAR_VS,
	LAMBERT_RIMLIGHT_VS,
	TEXTURE_VS,
	TEXTURE_NORMAL_VS,
	TEXTURE_NORMAL_GLOSS_VS,
	TEXTURE_ENVIRONMENT_VS,
	GEO_TEST_VS,
	TEXTURE_TESS_MODEL_VS,
	TEXTURE_TESS_TERRAIN_VS,
	TEXTURE_TESS_FLUID_TERRAIN_VS,
	GRASS_VS,
	DEFERRED1_VS,
	SHADOWMAP_VS,
	SHADOWMAP_TERRAIN_VS,
	TEXTURE_TESS_3SPLAT_VS,
	TERRAIN_TEST_VS,
	TERRAIN_LD_VS,
	TERRAIN_MD_VS,
	TERRAIN_LD_TOON_VS,
	ATMOSPHERE_VS,
	TEXTURE_TESS_3SPLAT_DEMO_VS,
	WHITE_NOISE_VS,
	VALUE_NOISE_VS,
	PERLIN_NOISE_VS,
	DYNAMIC_NOISE_VS,
	PERLIN_VORONOI_NOISE_VS,
	VOLUME_CLOUD_VS,
	_3DTEX_VS,
	WEATHER_MAP_VS,
	WEATHER_ATMOSPHERE_VS,
	SIMPLE_STAGE_VS,
	TRIPLANAR_TEXTURE_VS,
	TOON_MODEL_VS,
	TOON_MODEL_TEX_VS,
};

enum GS
{
	GSNONE,
	STANDARD_GS,
	HULL_GS,
	GRASS_GS,
};

enum HS
{
	HSNONE,
	STANDARD_HS,
	_2DISPLACE_HS,
	GRASS_HS,
	DEFERRED1_HS,
	SHADOWMAP_TERRAIN_HS,
	TEXTURE_TESS_3SPLAT_HS
};

enum DS
{
	DSNONE,
	STANDARD_DS,
	_2DISPLACE_DS,
	GRASS_DS,
	DEFERRED1_DS,
	SHADOWMAP_TERRAIN_DS,
	TEXTURE_TESS_3SPLAT_DS
};

enum Shaders
{
	//DIRECT SHADERS
	FLAT,
	FLAT_TEX,
	LAMBERT,
	LAMBERT_SPECULAR,
	LAMBERT_RIMLIGHT,
	TEXTURE,
	TEXTURE_NORMAL,
	TEXTURE_NORMAL_GLOSS,
	TEXTURE_ENVIRONMENT,
	GEO_TEST,
	TEXTURE_TESS_MODEL,
	TEXTURE_TESS_TERRAIN,
	TEXTURE_TESS_FLUID_TERRAIN,
	DEFERRED1,
	SHADOWMAP,
	SHADOWMAP_TERRAIN,
	TEXTURE_TESS_3SPLAT,
	TEXTURE_TESS_3SPLAT_GRASS,
	TERRAIN_TEST,
	TERRAIN_LD,
	TERRAIN_MD,
	TERRAIN_LD_TOON,
	TERRAIN_MD_TOON,
	TERRAIN_HD_TOON,
	ATMOSPHERE,
	TESSDEMO,
	WHITE_NOISE,
	VALUE_NOISE,
	PERLIN_NOISE,
	DYNAMIC_NOISE,
	PERLIN_VORONOI_NOISE,
	VOLUME_CLOUD,
	_3DTEX,
	WEATHER_MAP,
	WEATHER_ATMOSPHERE,
	SIMPLE_STAGE,
	TRIPLANAR_TEXTURE,
	TOON_MODEL,
	TOON_TEX_MODEL,
	SHADERMAX
};

enum FinalPassShaders
{
	TEST,
};

#define MAXBONES (32)