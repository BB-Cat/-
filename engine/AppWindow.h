#pragma once

#include "Window.h"
#include "high_resolution_timer.h"
#include "InputListener.h"

#include "Prerequisites.h"
#include "GraphicsEngine.h"
#include "SceneManager.h"



class AppWindow : public Window, public InputListener
{
public:

	//reset the input values saved in the app window
	void resetInput();
	//ImGui driven main menu function
	void mainMenu();

	//inherited from Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;
	virtual void onDestroy() override;
	//inherited from InputListener
	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;
	virtual void onMouseMove(const Point& delta_mouse_pos) override;
	virtual void onMouseLeftDown(const Point& mouse_pos) override;
	virtual void onMouseRightDown(const Point& mouse_pos) override;
	virtual void onMouseLeftUp(const Point& mouse_pos) override;
	virtual void onMouseRightUp(const Point& mouse_pos) override;

	
public:
	//static input data retrieval functions
	static bool getKeyState(int i);
	static bool getKeyTrigger(int i);
	static bool getKeyRelease(int i);
	//mouse data retrieval
	static Vector2D getMouseDelta();
	static bool getMouseState(bool rightbutton);
	static bool getMouseTrigger(bool rightbutton);
	static bool getMouseRelease(bool rightbutton);
	static Vector2D getScreenSize();

	static void toggleDeferredPipeline(bool type) { m_is_deferred_pipeline = type; }

private:
	SwapChainPtr m_swap_chain;
	SamplerPtr m_sampler;
	bool m_mouse = false;

private:
	static bool m_keys_state[256];
	static bool m_old_keys_state[256];
	static Vector2D m_delta_rot;
	static bool m_mouse_state[2];
	static bool m_old_mouse_state[2];
private:
	static Vector2D m_screen_size;

private:
	high_resolution_timer m_delta_time;
	std::unique_ptr<SceneManager> m_scene_manager;

private:
	static bool m_is_deferred_pipeline;

};

