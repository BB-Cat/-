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
			Walk,
			Run,
			Jump,
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