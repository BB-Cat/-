#include "AppWindow.h"
#include <Windows.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4X4.h"
#include "InputSystem.h"
#include "SkinnedMesh.h"
#include "ConstantBufferSystem.h"
#include "Lighting.h"
#include "TextRenderer.h"
#include "GBuffer.h"

#include "SwapChain.h"
#include "DeviceContext.h"
#include "InputListener.h"
#include "CameraManager.h"
//#include "SceneManager.h"
#include "ActorManager.h"
#include "WorldObjectManager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include <chrono>


struct vertex
{
	Vec3 position;
	Vec2 texcoord;
};

bool AppWindow::m_mouse = false;
bool AppWindow::m_keys_state[256] = {};
bool AppWindow::m_old_keys_state[256] = {};
Vec2 AppWindow::m_delta_rot = Vec2();
bool AppWindow::m_mouse_state[2] = {};
bool AppWindow::m_old_mouse_state[2] = {};

bool AppWindow::m_is_deferred_pipeline = true;

Vec2 AppWindow::m_screen_size = Vec2(0, 0);


std::shared_ptr<std::thread> AppWindow::m_compute_thread = nullptr;
bool AppWindow::m_compute_is_busy = false;
bool AppWindow::m_compute_is_queued = false;

std::function<void()>* AppWindow::m_func = nullptr;


AppWindow::~AppWindow()
{
	if (m_compute_thread != nullptr)
	{
		m_compute_thread->join();
		m_compute_thread.reset();
	}

	GraphicsEngine::get()->getRenderSystem()->releaseImGui();

	CameraManager::release();
	Lighting::release();
	ActorManager::release();
	WorldObjectManager::release();
}

void AppWindow::resetInput()
{
	std::memcpy(m_old_keys_state, m_keys_state, sizeof(bool) * 256);
	memset(m_keys_state, false, sizeof(bool) * 256);

	m_delta_rot.x = 0;
	m_delta_rot.y = 0;

	m_old_mouse_state[0] = m_mouse_state[0];
	m_old_mouse_state[1] = m_mouse_state[1];
}

void AppWindow::mouseToggleCheck()
{
	if (AppWindow::getKeyTrigger(VK_ESCAPE))
	{
		toggleMouse();
	}
}

void AppWindow::mainMenu()
{
	ImGui::BeginMainMenuBar();
	ImGui::Text(("FPS: " + std::to_string(m_fps)).c_str());

	if (ImGui::Button("Explanation")) m_show_popup = true;
	if (m_show_popup)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vec2 size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.x / 2, size.y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Welcome");
		ImGui::BeginPopupModal("Welcome");
		ImGui::TextWrapped("ESC: Toggle Mouse Display   -   Please use (ESC) key to turn on/off camera controls!");

		ImGui::NewLine();
		ImGui::TextWrapped("Welcome to my project!  I'm sorry, since ImGui doesn't support Japanese I am writing comments in English.  My project is still simple, but I want to keep improving it and become a great programmer.  Thank you for looking!");
		ImGui::NewLine();
		ImGui::TextWrapped("This program uses many text files for terrain. Please do not move or replace them or the program will crash.");
		//ImGui::Image(t, ImVec2(300, 300));
		if (ImGui::Button("Okay", ImVec2(100, 30))) m_show_popup = false;
		ImGui::EndPopup();
	}

	if (ImGui::BeginMenu("Shaders"))
	{
		GraphicsEngine::get()->getShaderManager()->imGuiMenuShaderCompile();
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void AppWindow::fpsTimer()
{
	m_fps_update_timer += m_delta_time.time_interval();
	m_loop_count++;
	if (m_fps_update_timer > 0.5f)
	{
		//calculate the average fps every 0.5 seconds
		float fps = (1.0f / (m_fps_update_timer / m_loop_count));
		if (fps - (int)(fps) >= 0.5f) m_fps = fps + 1.0f;
		else m_fps = fps;

		m_fps_update_timer = 0;
		m_loop_count = 0;
	}
}

bool AppWindow::queryComputeThread()
{
	if (m_compute_is_busy || m_compute_is_queued) return false;

	return true;
}

bool AppWindow::loadComputeThreadFunction(void (*func)())
{
	//std::function<void()> temp = func;
	//m_func = &temp;
	m_func = new std::function<void()>;
	*m_func = func;
	m_compute_is_queued = true;

	//std::shared_ptr<std::thread> t(new std::thread(temp));
	//t->join();
	
	return true;
}

void AppWindow::handleActiveComputeThreads()
{
	if (m_compute_thread != nullptr)
	{
		m_compute_thread->join();
		m_compute_thread.reset();
		m_compute_is_queued = false;
	}
}

void AppWindow::initiateComputeThreads()
{
	if (m_compute_is_queued)
	{
		std::shared_ptr<std::thread> t(new std::thread(*m_func));
		m_compute_thread = t;
	}
}

void AppWindow::onCreate()
{

	Window::onCreate();

	//set input listeners
	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(m_mouse);


	//create the swapchain
	RECT rc = Window::getClientWindowRect();
	m_screen_size = Vec2(rc.right - rc.left, rc.bottom - rc.top);
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, m_screen_size.x, m_screen_size.y);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->saveSwapChain(this->m_swap_chain);

	//initialize the gbuffer
	GraphicsEngine::get()->getRenderSystem()->createGBuffer(m_screen_size.x, m_screen_size.y);

	//initialize the text rendering system
	//GraphicsEngine::get()->createTextRenderer(&m_hwnd, m_swap_chain);

	//create the scene manager
	m_scene_manager = std::make_unique<SceneManager>();

	//compile runtime shaders
	GraphicsEngine::get()->getShaderManager()->compileShaders();

	//initialize the generic sampler
	m_sampler = GraphicsEngine::get()->getRenderSystem()->createSamplerState();

	//initialize constant buffers
	GraphicsEngine::get()->getConstantBufferSystem()->initializeConstantBuffers();

	//initalize the ImGui interface
	GraphicsEngine::get()->getRenderSystem()->initializeImGui(m_hwnd);

	//initialize variables for timing FPS
	m_fps_update_timer = 0;
	m_loop_count = 0;
	m_fps = 0;
}

void AppWindow::onUpdate()
{
	fpsTimer();
	Window::onUpdate();
	InputSystem::get()->update();

	DeviceContextPtr context = GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext();

	//clear render targets
	context->clearRenderTargetColor(this->m_swap_chain, 0.3f, 0.3f, 0.4f, 1);
	context->clearGBufferRenderTargetColor(this->m_swap_chain, 0.6f, 0.6f, 0.5f, 1);

	//set the size of the viewport for this frame
	context->setViewportSize(m_screen_size.x, m_screen_size.y);

	//set the scene lighting buffer
	GraphicsEngine::get()->getConstantBufferSystem()->setGlobalLightPropertyBuffer();

	//set the default sampler
	context->setSamplerState(m_sampler);

	//update and render the current scene
	m_scene_manager->execute(m_delta_time.time_interval(), m_screen_size.x, m_screen_size.y);

	//begin ImGui UI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//render imGui from the current scene
	m_scene_manager->imGui();

	//render the menu bar
	mainMenu();
	
	//finish rendering imgui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//if there is a compute shader that ran since the last frame, we need to join it now
	handleActiveComputeThreads();

	EnterCriticalSection(&GraphicsEngine::get()->getRenderSystem()->m_critical_section);

	//present the rendered chain
	m_swap_chain->present(true);

	LeaveCriticalSection(&GraphicsEngine::get()->getRenderSystem()->m_critical_section);

	//if there is a compute shader waiting to be queued, begin it now
	initiateComputeThreads();

	//increase the high resolution timer tick
	m_delta_time.tick();

	//check if the mouse should be visible or not
	mouseToggleCheck();

	//reset input data structures for the next frame
	resetInput();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
	GraphicsEngine::get()->getShaderManager()->recompileErrorShaders();
	m_delta_time.start();
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
	m_delta_time.stop();
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

void AppWindow::onKeyDown(int key)
{
	m_keys_state[key] = true;
}

void AppWindow::onKeyUp(int key)
{
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	if (m_mouse) return;

	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_delta_rot.x = (mouse_pos.m_y - (height / 2.0f)) * m_delta_time.time_interval() * 0.05f;
	m_delta_rot.y = (mouse_pos.m_x - (width / 2.0f)) * m_delta_time.time_interval() * 0.05f;
	InputSystem::get()->setCursorPosition(Point((int)(width / 2.0f), (int)(height / 2.0f)));
}

void AppWindow::onMouseLeftDown(const Point& mouse_pos)
{
	m_mouse_state[0] = true;
}

void AppWindow::onMouseRightDown(const Point& mouse_pos)
{
	m_mouse_state[1] = true;
}

void AppWindow::onMouseLeftUp(const Point& mouse_pos)
{
	m_mouse_state[0] = false;
}

void AppWindow::onMouseRightUp(const Point& mouse_pos)
{
	m_mouse_state[1] = false;
}

bool AppWindow::getKeyState(int i)
{
	return (m_keys_state[i] == true && m_old_keys_state[i] == true);
}

bool AppWindow::getKeyTrigger(int i)
{
	return (m_keys_state[i] == true && m_old_keys_state[i] == false);
}

bool AppWindow::getKeyRelease(int i)
{
	return (m_keys_state[i] == false && m_old_keys_state[i] == true);
}

Vec2 AppWindow::getMouseDelta()
{
	return m_delta_rot;
}

bool AppWindow::getMouseState(bool rightbutton)
{
	if (rightbutton) return (m_mouse_state[1] == true && m_old_mouse_state[1] == true);
	else return (m_mouse_state[0] == true && m_old_mouse_state[0] == true);
}

bool AppWindow::getMouseTrigger(bool rightbutton)
{
	if (rightbutton) return (m_mouse_state[1] == true && m_old_mouse_state[1] == false);
	else return (m_mouse_state[0] == true && m_old_mouse_state[0] == false);
}

bool AppWindow::getMouseRelease(bool rightbutton)
{
	if (rightbutton) return (m_mouse_state[1] == false && m_old_mouse_state[1] == true);
	else return (m_mouse_state[0] == false && m_old_mouse_state[0] == true);
}

Vec2 AppWindow::getScreenSize()
{
	return m_screen_size;
}

void AppWindow::toggleMouse()
{
	m_mouse = !m_mouse;
	if (m_mouse) InputSystem::get()->showCursor(true);
	else InputSystem::get()->showCursor(false);
}
