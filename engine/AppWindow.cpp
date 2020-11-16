#include "AppWindow.h"
#include <Windows.h>
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

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include <chrono>


struct vertex
{
	Vector3D position;
	Vector2D texcoord;
};

bool AppWindow::m_keys_state[256] = {};
bool AppWindow::m_old_keys_state[256] = {};
Vector2D AppWindow::m_delta_rot = Vector2D();
bool AppWindow::m_mouse_state[2] = {};
bool AppWindow::m_old_mouse_state[2] = {};

bool AppWindow::m_is_deferred_pipeline = true;

Vector2D AppWindow::m_screen_size = Vector2D(0, 0);


void AppWindow::resetInput()
{
	std::memcpy(m_old_keys_state, m_keys_state, sizeof(bool) * 256);
	memset(m_keys_state, false, sizeof(bool) * 256);

	m_delta_rot.m_x = 0;
	m_delta_rot.m_y = 0;

	m_old_mouse_state[0] = m_mouse_state[0];
	m_old_mouse_state[1] = m_mouse_state[1];
}

void AppWindow::mainMenu()
{
	ImGui::BeginMainMenuBar();
	ImGui::Text(("FPS: " + std::to_string(m_fps)).c_str());

	if (ImGui::Button("Explanation")) m_show_popup = true;
	if (m_show_popup)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		Vector2D size = AppWindow::getScreenSize();

		ImGui::SetNextWindowPos(ImVec2(size.m_x / 2, size.m_y / 2), 0, ImVec2(0.5f, 0.5f));
		//ImTextureID t = m_tex1->getSRV();

		ImGui::OpenPopup("Welcome");
		ImGui::BeginPopupModal("Welcome");
		ImGui::Text("1: Toggle Mouse Display");
		ImGui::Text("WASD: Camera Movement / Player Movement");
		ImGui::Text("Shift: Player Sprint");
		ImGui::Text("Space: Player Jump");

		ImGui::TextWrapped("This program uses many text files. Please do not move or replace them or the program will crash.");
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

void AppWindow::onCreate()
{
	Window::onCreate();

	//set input listeners
	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(m_mouse);


	//create the swapchain
	RECT rc = Window::getClientWindowRect();
	m_screen_size = Vector2D(rc.right - rc.left, rc.bottom - rc.top) * 1.0f;
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->saveSwapChain(this->m_swap_chain);

	//initialize the gbuffer
	GraphicsEngine::get()->getRenderSystem()->createGBuffer(m_screen_size.m_x, m_screen_size.m_y);

	//initialize the text rendering system
	GraphicsEngine::get()->createTextRenderer(&m_hwnd, m_swap_chain);

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
	m_fps_update_timer += m_delta_time.time_stamp();
	m_loop_count++;
	if (m_fps_update_timer > 0.5f)
	{
		//calculate the average fps every 0.5 seconds
		//m_fps = (m_fps_update_timer / m_loop_count) / 1.0f;
		m_fps = (1.0f / (m_fps_update_timer / m_loop_count));
		m_fps_update_timer = 0;
		m_loop_count = 0;
	}
	m_delta_time.reset();


	Window::onUpdate();
	InputSystem::get()->update();

	//clear render target
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0.3f, 0.3f, 0.4f, 1);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearGBufferRenderTargetColor(this->m_swap_chain, 0.6f, 0.6f, 0.5f, 1);

	//set the size of the viewport for this frame
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(m_screen_size.m_x, m_screen_size.m_y);

	//set the scene lighting buffer
	GraphicsEngine::get()->getConstantBufferSystem()->setGlobalLightPropertyBuffer();

	//set the lights buffer
	Lighting::get()->setLights();
	GraphicsEngine::get()->getConstantBufferSystem()->setLocalLightPropertyBuffer();
	//set the sampler
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setSamplerState(m_sampler);

	//render the gameworld
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	//update and render the current scene
	m_scene_manager->execute(m_delta_time.time_interval(), width, height);

	//begin ImGui UI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	//render imGui from the current scene
	m_scene_manager->imGui();
	//render the menu bar
	mainMenu();
	
	//ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



	////render final pass if the current pipeline is deferred
	//if (m_is_deferred_pipeline)
	//{
	//	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRenderTargetDirect();
	//	GraphicsEngine::get()->getRenderSystem()->getGBuffer()->renderToSwapChain(0, TEST, Vector2D(0.0f, 1.0f), Vector2D(0.5f, 0.5f), Vector2D(0, 0));
	//	GraphicsEngine::get()->getRenderSystem()->getGBuffer()->renderToSwapChain(1, TEST, Vector2D(1.0f, 1.0f), Vector2D(0.5f, 0.5f), Vector2D(0, 0));
	//	GraphicsEngine::get()->getRenderSystem()->getGBuffer()->renderToSwapChain(2, TEST, Vector2D(0.0f, 0.0f), Vector2D(0.5f, 0.5f), Vector2D(0, 0));
	//	GraphicsEngine::get()->getRenderSystem()->getGBuffer()->renderToSwapChain(3, TEST, Vector2D(1.0f, 0.0f), Vector2D(0.5f, 0.5f), Vector2D(0, 0));
	//}

	//static int showShadowMap = 2;
	//if (AppWindow::getKeyTrigger('V')) showShadowMap = (showShadowMap + 1) * (showShadowMap + 1 < 6) + 2 * (showShadowMap + 1 > 6);

	//if(showShadowMap > 2) GraphicsEngine::get()->getRenderSystem()->getGBuffer()->renderToSwapChain(showShadowMap, TEST, Vector2D(0.0f, 0.0f), Vector2D(1.0f, 1.0f), Vector2D(0, 0));

	if (AppWindow::getKeyTrigger('1'))
	{
		m_mouse = !m_mouse;
		if (m_mouse) InputSystem::get()->showCursor(true);
		else InputSystem::get()->showCursor(false);
	}

	//present the rendered chain
	m_swap_chain->present(true);

	//increase the high resolution timer
	m_delta_time.tick();

	//reset input data structures for the next frame
	resetInput();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
	GraphicsEngine::get()->getShaderManager()->recompileErrorShaders();
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
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

	m_delta_rot.m_x = (mouse_pos.m_y - (height / 2.0f)) * m_delta_time.time_interval() * 0.05f;
	m_delta_rot.m_y = (mouse_pos.m_x - (width / 2.0f)) * m_delta_time.time_interval() * 0.05f;
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

Vector2D AppWindow::getMouseDelta()
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

Vector2D AppWindow::getScreenSize()
{
	return m_screen_size;
}