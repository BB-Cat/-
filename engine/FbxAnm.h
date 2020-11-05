#pragma once
#include "MyFbxManager.h"

//TODO: Add a class which inherits from FbxAnm that uses a different update function to allow special animations like jumping
// which needs to perform a different type of check to see if the animation loops or can be finished.

//probably also need a way for the animation to string into other specific animations for things like jump -> land 

class FbxAnm
{
public:
	FbxAnm() {}
	FbxAnm(const wchar_t* full_path, MyFbxManager* fbx_manager, bool looping = true, bool interruptable = true, int interruptable_frame = 0);
	~FbxAnm() {}

	//update the current frame and return FALSE if the animation is finished and does not loop
	bool update(float delta);

	//check if the animation can be interrupted currently
	bool getIfInterruptable();
	//get the sampling rate of the animation to determine blended animation update speeds
	float getSamplingTime() { return m_skeletons.sampling_time; }

	//reset the frame 
	void reset() { m_frame = 0; };

	//set the frame to a percentage of completion
	void setFrame(float percent);
	//get the percentage completion of the animation
	float getPercent() { return (float)(m_frame) / (float)(m_skeletons.size()); }

	//return the skeleton for the next frame in the animation and provide the number of bones through a pointer
	Skeleton getPose();


private:
	bool m_loops = true;
	bool m_interruptable = true;
	int  m_interruptable_frame = 0;
	int  m_frame = 0;

	SkeletalAnimation	m_skeletons;
};

