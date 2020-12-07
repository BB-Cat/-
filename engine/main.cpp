#include "AppWindow.h"
#include "InputSystem.h"

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	try
	{
		GraphicsEngine::create();
		InputSystem::create();
	}
	catch (...) { return -1; }

	{
		try
		{
			AppWindow app;
 			while (app.isRun());
		}
		catch (...) 
		{ 
			InputSystem::release();
			GraphicsEngine::release();
			OutputDebugString(L"FATAL ERROR!");
			return -1;
		}
	}

	InputSystem::release();
	GraphicsEngine::release();

	return 0;
}
