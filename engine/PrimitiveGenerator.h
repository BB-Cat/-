#pragma once
#include "Prerequisites.h"
#include "Cube.h"

//�v���~�e�B�u�W�I���g���[��ۊǂ���N���X�B�V�����v���~�e�B�u�𐶐����鎞�A���̃N���X����o�b�t�@�����B
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
	

private:
	bool initPrimitiveCube();

private:
	//Cube Buffer
	VertexBufferPtr		m_cube_vb;
	IndexBufferPtr		m_cube_ib;
};