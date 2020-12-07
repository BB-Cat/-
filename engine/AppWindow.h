#pragma once

#include "Window.h"
#include "high_resolution_timer.h"
#include "InputListener.h"

#include "Prerequisites.h"
#include "GraphicsEngine.h"
#include "SceneManager.h"

#include <thread>


class AppWindow : public Window, public InputListener
{
public:
	~AppWindow();

	//reset the input values saved in the app window
	void resetInput();
	//toggle whether to show the mouse
	void mouseToggleCheck();
	//ImGui driven main menu function
	void mainMenu();
	//time the FPS
	void fpsTimer();

public:  //functions for multithreading compute shaders in a way that does not harm the graphics pipeline
	//returns whether or not the AppWindow compute thread is available for use
	static bool queryComputeThread();
	//function which saves a function pointer to be started after the next render cycle is complete
	static bool loadComputeThreadFunction(void (*func)());



	static std::shared_ptr<std::thread> getComputeThread() { return m_compute_thread; }


public:

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

	static void toggleMouse();
	static void toggleDeferredPipeline(bool type) { m_is_deferred_pipeline = type; }

private:
	//function which waits for any unfinished compute shaders to join before continuing
	void handleActiveComputeThreads();
	//function which starts a multi threaded compute shader to be completed by time the next render cycle is about to finish
	void initiateComputeThreads();

private:
	SwapChainPtr m_swap_chain;
	SamplerPtr m_sampler;


private:
	static bool m_mouse;
	static bool m_keys_state[256];
	static bool m_old_keys_state[256];
	static Vector2D m_delta_rot;
	static bool m_mouse_state[2];
	static bool m_old_mouse_state[2];
private:
	static Vector2D m_screen_size;

private:
	high_resolution_timer m_delta_time;
	float m_fps_update_timer;
	int   m_loop_count;
	int   m_fps;

private: 
	//ImGui
	bool m_show_popup = true;
	TexturePtr m_image;

	std::unique_ptr<SceneManager> m_scene_manager;

private:
	//this thread pointer is used interfaced with by other classes to run a compute shader in the background
	static std::shared_ptr<std::thread> m_compute_thread;
	static bool m_compute_is_busy;
	static bool m_compute_is_queued;


	static std::function<void()>* m_func;

private:
	static bool m_is_deferred_pipeline;

};

