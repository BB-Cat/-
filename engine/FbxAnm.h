#pragma once
#include "MyFbxManager.h"

class Fbx_Anm
{
public:
	Fbx_Anm() {}
	Fbx_Anm(const wchar_t* full_path, MyFbxManager* fbx_manager, 
		bool looping = true, bool interruptable = true, int interruptable_frame = 0,
		bool idles = false, int idle_frame = 0);
	Fbx_Anm(const wchar_t* full_path, MyFbxManager* fbx_manager,
		bool looping = true, bool interruptable = true, float interruptable_percent = 0,
		bool idles = false, float idle_percent = 0);
	~Fbx_Anm() {}

	//update the current frame and return FALSE if the animation is finished and does not loop
	bool update(float delta);
	//update the current frame using a percentage of the total length (used in animation blending)
	bool updatedByPercentage(float percent);

	//==============================================================//
			/* �e�X�V�����֐��@*/
	bool updateLooping();
	bool updateNonLooping();
	bool updateIdling();
	//==============================================================//


	//check if the animation can be interrupted currently
	bool getIfInterruptable();

	//check if the animation has finished playing
	bool getIfFinished();

	//get the total length of the animation to blend animations together
	float getTotalTime();

	float getPercentCompletion();

	//reset the animation to default 
	void reset();

	//set the frame to a specific frame
	void setFrame(int  frame);

	//set the frame to a percentage of completion
	void setFrame(float percent);

	//set the trigger to complete an idling animation
	void setFinishTrigger(bool trigger);

	//get the percentage completion of the animation
	float getPercent() { return (float)(m_frame) / (float)(m_skeletons.size()); }

	//return the skeleton for the next frame in the animation and provide the number of bones through a pointer
	Skeleton getPose();
	//return the skeleton of a specific frame percentage in the animation
	Skeleton getPoseAtPercent(float percent);


protected:
	//���݂̃t���[��
	int  m_frame = 0;
	//�J��Ԃ����ǂ�����ݒ肷��ϐ�
	bool m_loops;
	//�r�����甲���邩�ǂ���
	bool m_interruptable;
	//�r�����甲����Ȃ牽�t���[�����甲����̐ݒ�
	int  m_interruptable_frame;
	//�r���ōX�V���~�߂邩�ǂ���
	bool m_idles;
	//�r���ōX�V��ҋ@����A�j���[�V�����̏ꍇ�A���t���[���܂Ő���������~�܂邩�̐ݒ�
	int  m_idle_frame;
	//�r���ōX�V��ҋ@����A�j���[�V�����ł���΁A���̕ϐ���TRUE�ɂȂ������̂ݏI������
	bool m_trigger_finish = false;

	Skeletal_Animation	m_skeletons;
};

