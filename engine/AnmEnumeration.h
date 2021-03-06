#pragma once


//namespace enclosed enumerations used to initialize animation vectors
namespace Animation
{
	namespace Type
	{
		enum
		{
			Player,
			Humanoid,
			MAX
		};
	}

	namespace Player
	{
		enum 
		{
			Idle,
			Idle2,
			Idle3,
			Walk,
			WalkBackward,
			Run,
			StrafeRight,
			StrafeLeft,
			Stop,
			Jump,
			LandToIdle,
			LandHard,
			LandToRun,
			Roll,
			DodgeBack,
			Attack1,
			Attack2,
			Attack3,
			Attack4,
			Attack5,
			MAX
		};
	}

	namespace Humanoid
	{
		enum
		{
			Idle,
			Walk,
			MAX
		};
	}

}