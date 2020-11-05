#include "window.h"
#include <exception>


//Window* window = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwndn, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

	switch (msg)
	{
	case WM_CREATE:
	{
		break;
	}
	case WM_SETFOCUS:
	{
		// Event fired when the window is focused
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(window) window->onFocus();
		break;
	}
	case WM_KILLFOCUS:
	{
		// Event fired when the window loses focus
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->onKillFocus();
		break;
	}
	case WM_DESTROY:
	{
		// Event fired when the window is destroyed
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->onDestroy();
		::PostQuitMessage(0);
		break;
	}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return NULL;
}




Window::Window()
{
	//Setting up WNDCLASSEX object

	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = NULL;
	wc.lpszClassName = L"•`‰æƒGƒ“ƒWƒ“";
	wc.lpszMenuName = L"";
	wc.style = NULL;
	wc.lpfnWndProc = &fnWndProc;


	// If registration of class fails, the function will throw an exception
	if (!::RegisterClassEx(&wc))
		throw std::exception("Registration of the windows class has failed");

	//Creation of the window
	m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"•`‰æƒGƒ“ƒWƒ“", L"•`‰æƒGƒ“ƒWƒ“", WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, NULL, NULL);

	//if the creation of the window fails throw an exception
	if (!m_hwnd)
		throw std::exception("Creation of the window has failed");

	//display the window
	::ShowWindow(m_hwnd, SW_SHOW);
	//update the window
	::UpdateWindow(m_hwnd);

	//set this flag to true to indicate that the window is initialized and running
	m_is_run = true;
}

Window::~Window()
{
}

bool Window::broadcast()
{
	MSG msg;

	if (!this->m_is_init)
	{
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
		this->onCreate();
		m_is_init = true;
	}

	this->onUpdate();

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	Sleep(1);

	return true;
}

bool Window::release()
{

	return true;
}

bool Window::isRun()
{
	if (m_is_run) broadcast();
	return m_is_run;
}

RECT Window::getClientWindowRect()
{
	RECT rc;
	::GetClientRect(m_hwnd, &rc);

	return rc;
}

void Window::onCreate()
{
}

void Window::onUpdate()
{

}

void Window::onDestroy()
{
	m_is_run = false;
}

void Window::onFocus()
{
}

void Window::onKillFocus()
{
}
