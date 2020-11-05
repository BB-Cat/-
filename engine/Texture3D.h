#pragma once
#include "Resource.h"
#include <d3d11.h>
#include "Vector2D.h"
#include "Vector3D.h"

#include <vector>

/*
�������3�����e�N�X�`���ł��B
���݂RD�e�N�X�`���̎g�����Ƃ��ĉ_�̃{�����[���g���b�N�v�Z�ɂ����g���\�肪�Ȃ��ׁA
�ʂ̃��\�[�X�ƈ���Đ�p�̃��\�[�X�}�l�W���[�����݂��܂���B

This is the 3D texture class.
It won't be used for anything besides volumetric rendering (for now)
so it does not have a resource manager counterpart.  If 3D textures become useful for other things
in the future I will make a manager then.
*/

class Texture3D
{
public:
	//�v�Z�ς݂̃f�[�^�[��p���ăe�N�X�`���[�����
	Texture3D(Vector3D dimensions, const std::vector<float>& data);
	//�t�@�C�����珀�����Ă���3�����e�N�X�`���[��ǂݍ���
	Texture3D(std::string filename);
	~Texture3D();

	ID3D11ShaderResourceView* getShaderResourceView() { return m_shader_res_view; }

	//txt�t�@�C���Ɍ��݂̃f�[�^�[���o�͂���
	void outputFile();

private:
	//txt�t�@�C������f�[�^�[��ǂݍ���
	void loadFile(std::string filename);

private:
	ID3D11ShaderResourceView* m_shader_res_view = nullptr;
	std::vector<float> m_data;

	int m_height;
	int m_width;
	int m_depth;
private:
	friend class DeviceContext;
};

