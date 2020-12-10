#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include "SkinnedMesh.h"

class Actor
{
public:
	Actor(bool has_shadow = false) : m_has_shadow(has_shadow) {}

	Vec3 getPosition() { return m_pos; };
	Vec3 getDirectionVector() { return Vec3(sinf(m_angle * 0.01745f), 0, cosf(m_angle * 0.01745f)); };
	Vec3 getRightVector() { return Vec3(sinf((m_angle + 90.0f) * 0.01745f), 0, cosf((m_angle + 90.0f) * 0.01745f)); };

	virtual void update(float delta) = 0;
	virtual void render(float delta) = 0;
	virtual void renderShadow(float delta) = 0;

	virtual void imGuiWindow() {}
	//virtual void animationTree() {}
protected:
	Vec3 m_pos;
	Vec3 m_rot;
	Vec3 m_scale;
	float m_angle;
	SkinnedMeshPtr m_mesh;
	//state�̏����l�̂O���S�ẴA�j���[�V�����l�[���X�y�[�X�ł͑ҋ@�Ƃ����Ӗ������̂ŁA������Ԃ��K���O�ł��B
	int m_state = 0;
	//�A�j���[�V�����̍X�V��previous state��state�̒l���قȂ����ꍇ�̂ݍX�V����̂ŁA�ŏ��̑ҋ@�A�j���[�V�������n�߂�̂Ɂ]1�ɐݒ肵�܂��B
	int m_previous_state = -1;

	////�A�j���[�V�����֘A
	//int m_try_animation = -1;
	//int m_queued_animation = -1;

	//�f�B�t�@�h�����_�����O�Ɋւ���ϐ�
	bool m_has_shadow;

protected:
	friend class ActorManager;
};
