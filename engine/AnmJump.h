#pragma once
#include "FbxAnm.h"
#include "Actor.h"

//class AnmJump : public FbxAnm
//{
//public:
//	AnmJump(const wchar_t* full_path, MyFbxManager* fbx_manager, bool looping = true, bool interruptable = true, int interruptable_frame = 0);
//
//	//update the current frame and return FALSE if the animation is finished and does not loop
//	bool update(float delta) override;
//private:
//	std::shared_ptr<Actor> m_actor;
//};