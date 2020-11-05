#include "FbxAnm.h"

FbxAnm::FbxAnm(const wchar_t* full_path, MyFbxManager* fbx_manager, bool looping, bool interruptable, int interruptable_frame) :
	m_loops(looping), m_interruptable(interruptable), m_interruptable_frame(interruptable_frame)
{
	fbx_manager->loadAnimationData(full_path, m_skeletons, 60);
}

bool FbxAnm::update(float delta)
{
	//update the animation tick
	m_skeletons.animation_tick += delta;

	//calculate which frame is next
	m_frame = m_skeletons.animation_tick / m_skeletons.sampling_time;

	//reset the animation if we passed the last frame
	if (m_frame > m_skeletons.size() - 1)
	{
		m_frame = 0;
		m_skeletons.animation_tick = 0;

		if (m_loops == false) return false;
	}

	return true;
}

bool FbxAnm::getIfInterruptable()
{
	//if the animation is interruptable and at the frame where it can be interrupted, return true
	return (m_interruptable && m_frame >= m_interruptable_frame);
}

void FbxAnm::setFrame(float percent)
{
	m_frame = (int)(percent * m_skeletons.size());
}

Skeleton FbxAnm::getPose()
{
	return m_skeletons.at(m_frame);
}
