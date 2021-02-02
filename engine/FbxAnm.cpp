#include "FbxAnm.h"

#include <string>

Fbx_Anm::Fbx_Anm(const wchar_t* full_path, MyFbxManager* fbx_manager, bool looping, 
	bool interruptable, int interruptable_frame, bool idles, int idle_frame, bool try_preloaded_txt) :
	m_loops(looping), m_interruptable(interruptable), m_interruptable_frame(interruptable_frame), 
	m_idles(idles), m_idle_frame(idle_frame)
{
	m_file_location = full_path;

	if (!(try_preloaded_txt && tryLoadAnimationTxt(full_path)))
	{
		fbx_manager->loadAnimationData(full_path, m_skeletons, 60);
	}


}

Fbx_Anm::Fbx_Anm(const wchar_t* full_path, MyFbxManager* fbx_manager, bool looping, 
	bool interruptable, float interruptable_percent, bool idles, float idle_percent, bool try_preloaded_txt) :
	m_loops(looping), m_interruptable(interruptable), m_idles(idles)
{
	m_file_location = full_path;

	//if (!try_preloaded_txt || !tryLoadAnimationTxt(full_path))
	//{
		int updates_per_second = 60;
		fbx_manager->loadAnimationData(full_path, m_skeletons, updates_per_second);

		m_interruptable_frame = (int)(interruptable_percent * m_skeletons.size());
		m_idle_frame = (int)(idle_percent * m_skeletons.size());

		//save the loaded data for next time
		//outputAnimationData();
	//}
}

Fbx_Anm::~Fbx_Anm()
{
	m_hitboxes.clear();

	Skeletal_Animation	m_skeletons;
	
	for (int i = 0; i < m_skeletons.size(); i++) m_skeletons[i].m_bones.clear();
	m_skeletons.clear();
}

bool Fbx_Anm::tryLoadAnimationTxt(const wchar_t* full_path)
{
	std::wstring txt_file_path(full_path);
	UINT path_len = wcslen(full_path);
	//cut off the .fbx at the end of the path and replace it with .txt
	txt_file_path.resize(path_len - 3);
	txt_file_path += L"txt";

	//std::string file = filename;
	std::ifstream fin;
	//fin = std::ifstream("..\\Assets\\SceneData\\" + file);
	fin = std::ifstream(txt_file_path);

	//_ASSERT_EXPR(fin.is_open(), L"Scene Data file not found!");
	if (!fin.is_open()) return false;


	//int num_tex;
	//int num_prefabs;
	//std::string tempname = {};
	//wchar_t tempwchar[128] = {};
	//std::string tempfile = {};

	//fin >> num_tex;

	std::string in;

	//m_loops
	fin >> m_loops;

	//m_interruptable
	fin >> m_interruptable;

	//m_interruptable_frame
	fin >> m_interruptable_frame;

	//m_idles
	fin >> m_idles;

	//m_idle_frame
	fin >> m_idle_frame;

	//HITBOXES
	//number of hitboxes
	int num_hitboxes;
	fin >> num_hitboxes;

	int active_frames[2] = {};
	int damage = 0;
	Collider* col;

	for (int i = 0; i < num_hitboxes; i++)
	{
		//active frames
		fin >> active_frames[0];
		fin >> active_frames[1];

		//damage	
		

		//collider
		
	}

	//SKELETONS
	//number of skeletons
	


	return true;
}

bool Fbx_Anm::outputAnimationData()
{
	std::wstring txt_file_path(m_file_location);
	txt_file_path.resize(m_file_location.size() - 3);
	txt_file_path += L"txt";

	std::ofstream outfile(txt_file_path.c_str());

	//m_loops
	if (m_loops) outfile << std::to_string(1);
	else outfile << std::to_string(0);

	//m_interruptable
	if (m_interruptable) outfile << std::to_string(1);
	else outfile << std::to_string(0);

	//m_interruptable_frame
	outfile << std::to_string(m_interruptable_frame) << " ";

	//m_idles
	if (m_idles) outfile << std::to_string(1);
	else outfile << std::to_string(0);

	//m_idle_frame
	outfile << std::to_string(m_idle_frame) << " ";

	//HITBOXES
	//number of hitboxes
	int num_hitboxes = m_hitboxes.size();
	outfile << std::to_string(num_hitboxes) << " ";

	for (int i = 0; i < num_hitboxes; i++)
	{
		//active frames
		outfile << std::to_string(m_hitboxes[i]->m_active_frames[0]) + " "
			+ std::to_string(m_hitboxes[i]->m_active_frames[1]);
		//damage
		outfile << std::to_string(m_hitboxes[i]->m_damage_value) + " ";

		//collider
		outputColliderData(m_hitboxes[i]->m_collider.get(), &outfile);
	}

	//SKELETONS
	//number of skeletons
	outputSkeletonData(&outfile);

	outfile.close();
	return true;
}

bool Fbx_Anm::outputColliderData(Collider* c, std::ofstream* stream)
{
	//determine type of collider
	//TODO: add support for other colliders besides capsules
	if (c->getType() != ColliderTypes::Capsule) return false;
	CapsuleCollider* c1 = reinterpret_cast<CapsuleCollider*>(c);

	//output appropriate data//

	//radius
	(*stream) << c1->getRadius() << " ";
	//core length
	(*stream) << c1->getCoreLength() << " ";
	//rotation
	Vec3 temp = c1->getRotation();
	(*stream) << std::to_string(temp.x) << " ";
	(*stream) << std::to_string(temp.y) << " ";
	(*stream) << std::to_string(temp.z) << " ";

	return true;
}

bool Fbx_Anm::outputSkeletonData(std::ofstream* stream)
{
	int num_skeletons = m_skeletons.size();
	//output the number of skeletons
	(*stream) << std::to_string(num_skeletons) + " ";

	if (num_skeletons == 0) return false;

	//output the number of bones in the skeleton 
	/* WE ARE ASSUMING THE NUMBER OF BONES NEVER CHANGES HERE */
	int num_bones = m_skeletons[0].m_bones.size();
	(*stream) << std::to_string(num_bones) + " ";

	Matrix4x4 mat;
	for (int i = 0; i < m_skeletons.size(); i++)
	{
		//output the matrix for each bone
		for (int j = 0; j < num_bones; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				for (int l = 0; l < 4; l++)
				{
					(*stream) << std::to_string(m_skeletons[i].m_bones[j].transform.m_mat[l][k]) << " ";
				}
			}
		}

	}
	return true;
}

bool Fbx_Anm::update(float delta)
{
	//待機状態を持つアニメーションであれば、更新出来るかを判定する。待機を持っていない場合はいつも更新する
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

bool Fbx_Anm::updatedByPercentage(float percent)
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

bool Fbx_Anm::updateLooping()
{
	//reset the animation if we passed the last frame
	if (m_frame > m_skeletons.size() - 1)
	{
		m_frame = 0;
		m_skeletons.animation_tick = 0;
	}

	return true;
}

bool Fbx_Anm::updateNonLooping()
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

bool Fbx_Anm::updateIdling()
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

bool Fbx_Anm::getIfInterruptable()
{
	//if the animation is interruptable and at the frame where it can be interrupted, return true
	return ((m_interruptable && m_frame >= m_interruptable_frame));
}

bool Fbx_Anm::getIfFinished()
{
	return (m_frame == m_skeletons.size() - 1);
}

float Fbx_Anm::getTotalTime()
{
	return (m_skeletons.sampling_time * m_skeletons.size());
}

float Fbx_Anm::getPercentCompletion()
{
	return  m_skeletons.animation_tick / (m_skeletons.sampling_time * m_skeletons.size());
}

void Fbx_Anm::reset()
{
	m_frame = 0;
	m_trigger_finish = false;
	m_skeletons.animation_tick = 0;
}

void Fbx_Anm::setFrame(int frame)
{
	m_frame = frame;
}

void Fbx_Anm::setFrame(float percent)
{
	m_frame = (int)(percent * m_skeletons.size());
}

void Fbx_Anm::setFinishTrigger(bool trigger)
{
	m_trigger_finish = trigger;
}

Skeleton Fbx_Anm::getPose()
{
	return m_skeletons.at(m_frame);
}

Skeleton Fbx_Anm::getPoseAtPercent(float percent)
{
	return m_skeletons.at((int)(percent * m_skeletons.size()));
}
