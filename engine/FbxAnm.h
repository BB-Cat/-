#pragma once
#include "MyFbxManager.h"
#include "AnmHitbox.h"

#include <iostream>
#include <fstream>  

class Fbx_Anm
{
public:
	Fbx_Anm() {}
	/* please note, if the "try_preloaded_txt" parameter is true animation data from the text files
	will be prioritized INSTEAD OF the other arguments in the constructor you are using (e.g. the looping
	variable will be false even if you entered true if the text file was saved as false originally) */
	Fbx_Anm(const wchar_t* full_path, MyFbxManager* fbx_manager, 
		bool looping = true, bool interruptable = true, int interruptable_frame = 0,
		bool idles = false, int idle_frame = 0, bool try_preloaded_txt = true);
	Fbx_Anm(const wchar_t* full_path, MyFbxManager* fbx_manager,
		bool looping = true, bool interruptable = true, float interruptable_percent = 0,
		bool idles = false, float idle_percent = 0, bool try_preloaded_txt = true);
	~Fbx_Anm();

	//initialization functions//

	//check the fbx file location for a text file with the same name.  if there is one present, try to load animation data.
	bool tryLoadAnimationTxt(const wchar_t* full_path);

	//output the animation matrixes and other variables to a text file for faster loading
	bool outputAnimationData();

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

	//get the percentage completion of the animation
	float getPercent() { return (float)(m_frame) / (float)(m_skeletons.size()); }

	int getTotalFrames() { return m_skeletons.size(); }

	//reset the animation to default 
	void reset();

	//set the animation to a specific frame
	void setFrame(int  frame);

	//set the animation to a percentage of completion
	void setFrame(float percent);

	//set the trigger to complete an idling animation
	void setFinishTrigger(bool trigger);

	//return the skeleton for the next frame in the animation and provide the number of bones through a pointer
	Skeleton getPose();
	//return the skeleton of a specific frame percentage in the animation
	Skeleton getPoseAtPercent(float percent);

	////////////////////
	//Hitbox Functions//

	//TODO: make this function insert hitboxes in order of active frame
	void addHitbox(std::shared_ptr<AnmHitbox> hb) { m_hitboxes.push_back(hb); }
	
	//retrieve the vector of hitboxes for rendering or other calculating collisions
	std::vector<std::shared_ptr<AnmHitbox>> getHitboxes() { return m_hitboxes; }

private:
	//output collider data to an existing output stream
	bool outputColliderData(Collider* c, std::ofstream* stream);
	//output all skeleton frame data to an existing output stream
	bool outputSkeletonData(std::ofstream* stream);

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
	//�����蔻��������x�N�^�[
	std::vector<std::shared_ptr<AnmHitbox>> m_hitboxes;

	Skeletal_Animation	m_skeletons;

	//�ۑ��p�̕ϐ�
	std::wstring m_file_location;
};

