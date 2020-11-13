#include "FbxAnm.h"

FbxAnm::FbxAnm(const wchar_t* full_path, MyFbxManager* fbx_manager, bool looping, 
	bool interruptable, int interruptable_frame, bool idles, int idle_frame) :
	m_loops(looping), m_interruptable(interruptable), m_interruptable_frame(interruptable_frame), 
	m_idles(idles), m_idle_frame(idle_frame)
{
	fbx_manager->loadAnimationData(full_path, m_skeletons, 60);
}

FbxAnm::FbxAnm(const wchar_t* full_path, MyFbxManager* fbx_manager, bool looping, 
	bool interruptable, float interruptable_percent, bool idles, float idle_percent) :
	m_loops(looping), m_interruptable(interruptable), m_idles(idles)
{
	fbx_manager->loadAnimationData(full_path, m_skeletons, 60);

	m_interruptable_frame = (int)(interruptable_percent * m_skeletons.size());
	m_idle_frame = (int)(idle_percent * m_skeletons.size());
}

bool FbxAnm::update(float delta)
{
	//待機状態を持つアニメーションであれば、更新出来るかを判定する。待機を持たない場合はいつも更新する
	if (m_idles == true)
	{
		if (m_frame < m_idle_frame || m_trigger_finish == true)
		{
			m_skeletons.animation_tick += delta;
		}
	}
	else m_skeletons.animation_tick += delta;

	//calculate which frame is next
	m_frame = m_skeletons.animation_tick / m_skeletons.sampling_time;

	if (m_idles == true) 
		return updateIdling();
	else if (m_loops == false) 
		return updateNonLooping();
	else if (m_loops == true) 
		return updateLooping();
	else return false;
}

bool FbxAnm::updatedByPercentage(float percent)
{
	float tick_increase = percent * (m_skeletons.sampling_time * m_skeletons.size());

	//待機状態を持つアニメーションであれば、更新出来るかを判定する。待機を持たない場合はいつも更新する
	if (m_idles == true)
	{
		if (m_frame < m_idle_frame || m_trigger_finish == true)
		{
			m_skeletons.animation_tick += tick_increase;
		}
	}
	else m_skeletons.animation_tick += tick_increase;

	//calculate which frame is next
	m_frame = m_skeletons.animation_tick / m_skeletons.sampling_time;

	if (m_idles == true)
		return updateIdling();
	else if (m_loops == false)
		return updateNonLooping();
	else if (m_loops == true)
		return updateLooping();
	else return false;
}

bool FbxAnm::updateLooping()
{
	//reset the animation if we passed the last frame
	if (m_frame > m_skeletons.size() - 1)
	{
		m_frame = 0;
		m_skeletons.animation_tick = 0;
	}

	return true;
}

bool FbxAnm::updateNonLooping()
{
	//if the animation is finished return false
	if (m_frame > m_skeletons.size() - 1)
	{
		m_frame = 0;
		m_skeletons.animation_tick = 0;
		return false;
	}

	return true;
}

bool FbxAnm::updateIdling()
{
	//if the animation is finished return false
	if (m_frame > m_skeletons.size() - 1)
	{
		m_frame = 0;
		m_skeletons.animation_tick = 0;
		m_trigger_finish = false;
		return false;
	}

	return true;
}

bool FbxAnm::getIfInterruptable()
{
	//if the animation is interruptable and at the frame where it can be interrupted, return true
	return ((m_interruptable && m_frame >= m_interruptable_frame));
}

bool FbxAnm::getIfFinished()
{
	return (m_frame == m_skeletons.size() - 1);
}

float FbxAnm::getTotalTime()
{
	return (m_skeletons.sampling_time * m_skeletons.size());
}

float FbxAnm::getPercentCompletion()
{
	return  m_skeletons.animation_tick / (m_skeletons.sampling_time * m_skeletons.size());
}

void FbxAnm::reset()
{
	m_frame = 0;
	m_trigger_finish = false;
	m_skeletons.animation_tick = 0;
}

void FbxAnm::setFrame(int frame)
{
	m_frame = frame;
}

void FbxAnm::setFrame(float percent)
{
	m_frame = (int)(percent * m_skeletons.size());
}

void FbxAnm::setFinishTrigger(bool trigger)
{
	m_trigger_finish = trigger;
}

Skeleton FbxAnm::getPose()
{
	return m_skeletons.at(m_frame);
}

Skeleton FbxAnm::getPoseAtPercent(float percent)
{
	return m_skeletons.at((int)(percent * m_skeletons.size()));
}
