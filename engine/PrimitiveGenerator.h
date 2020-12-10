#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include <vector>

//プリミティブジオメトリーを保管するクラス。新しいプリミティブを生成する時、このクラスからバッファを取る。
class PrimitiveGenerator
{
private:
	PrimitiveGenerator();
	static PrimitiveGenerator* instance;
public:
	static PrimitiveGenerator* get()
	{
		if (instance == nullptr) instance = new PrimitiveGenerator();
		return instance;
	}

	//========================================================================
	// UNIT FUNCTIONS - Returns a premade primitive with a height of exactly 1 
	/*(some primitives such as capsules have different widths)*/
	PrimitivePtr createUnitCube(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat);
	PrimitivePtr createUnitSphere(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat);
	PrimitivePtr createUnitCapsule(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat);
	//========================================================================
	// Functions to create brand new geometry with the given settings
	//PrimitivePtr createCustomCube(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat);
	PrimitivePtr createCustomSphere(float radius, int stacks, int slices, const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat);
	PrimitivePtr createCustomCapsule(float radius, float core_height, int stacks, int slices, const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat);


	void		initTextures(PrimitivePtr& output, const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, Material_Obj* mat);
	bool		loadTexture(std::wstring file, std::string name);
	TexturePtr	findTexture(std::string name);
	void		clearTextures();

	std::vector<TexturePtr> getTextures() { return m_textures; }
	std::vector<std::string> getTextureNames() { return m_tex_names; }
	std::vector<std::wstring> getTextureFiles() { return m_tex_files; }
	
	void getCubeBuffers(VertexBufferPtr vb, IndexBufferPtr ib) 
	{
		vb = m_cube_vb;
		ib = m_cube_ib;
	}

	bool generateSphereGeometry(float radius, int stacks, int slices, VertexBufferPtr& vb, IndexBufferPtr& ib);
	bool generateCapsuleGeometry(float radius, float core_height, int stacks, int slices, VertexBufferPtr& vb, IndexBufferPtr& ib);

private:
	bool initPrimitiveCube();
	bool initPrimitiveSphere();
	//this is probably going to need to be replaced with a more customizeable capsule creator
	bool initPrimitiveCapsule();

private:
	//Cube Buffer
	VertexBufferPtr		m_cube_vb;
	IndexBufferPtr		m_cube_ib;

	VertexBufferPtr     m_sphere_vb;
	IndexBufferPtr		m_sphere_ib;

	VertexBufferPtr     m_capsule_vb;
	IndexBufferPtr		m_capsule_ib;

	//Primitive Texture Pointers
	std::vector<TexturePtr> m_textures;
	std::vector<std::string> m_tex_names;
	//used to save texture file names for automatic loading later
	std::vector<std::wstring> m_tex_files;
};