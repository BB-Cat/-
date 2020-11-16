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

	CubePtr createCube(const wchar_t* diffuse_tex, const wchar_t* normal_tex, const wchar_t* roughness_tex, 
		Vector3D scale, Vector3D position, Vector3D rotation, Material_Obj* mat);

	bool loadTexture(std::wstring file, std::string name);
	TexturePtr findTexture(std::string name);
	void clearTextures();

	std::vector<TexturePtr> getTextures() { return m_textures; }
	std::vector<std::string> getTextureNames() { return m_tex_names; }
	std::vector<std::wstring> getTextureFiles() { return m_tex_files; }
	

private:
	bool initPrimitiveCube();

private:
	//Cube Buffer
	VertexBufferPtr		m_cube_vb;
	IndexBufferPtr		m_cube_ib;

	//Primitive Texture Pointers
	std::vector<TexturePtr> m_textures;
	std::vector<std::string> m_tex_names;
	//used to save texture file names for automatic loading later
	std::vector<std::wstring> m_tex_files;
};