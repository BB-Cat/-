#pragma once
#include <Windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>

#include "Vector2D.h"

class Window
{
public:




	//initialize the window
	Window();
	~Window();



	//release the window
	bool release();
	bool isRun();

	RECT getClientWindowRect();

	Vec2 getScreenSize() { return Vec2(SCREEN_WIDTH, SCREEN_HEIGHT); }

	//events
	virtual void onCreate()=0;
	virtual void onUpdate()=0;
	virtual void onFocus() = 0;
	virtual void onKillFocus() = 0;
	virtual void onDestroy()=0;

private:
	bool broadcast();
protected:
	HWND  m_hwnd;
	bool m_is_run;
	bool m_is_init = false;

	static const LONG SCREEN_WIDTH = 1024;
	static const LONG SCREEN_HEIGHT = 760;


protected:
	friend class TextRenderer;
};