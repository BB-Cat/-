#include "InputSystem.h"
#include <Windows.h>


InputSystem* InputSystem::m_system = nullptr;

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	m_system = nullptr;
}

void InputSystem::update()
{
	POINT current_mouse_pos = {};
	::GetCursorPos(&current_mouse_pos);

	if (m_first_time)
	{
		m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);
		m_first_time = false;
	}

	if (current_mouse_pos.x != m_old_mouse_pos.m_x || current_mouse_pos.y != m_old_mouse_pos.m_y)
	{
		//there has been a mouse movement event
		std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();

		while (it != m_set_listeners.end())
		{
			(*it)->onMouseMove(Point(current_mouse_pos.x, current_mouse_pos.y));
			++it;
		}
	}
	m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);

	if (::GetKeyboardState(m_keys_state))
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			if (m_keys_state[i] & 0x80) //KEY IS PRESSED
			{
				std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();

				while (it != m_set_listeners.end())
				{
					if (i == MK_LBUTTON)
					{
						if (m_keys_state[i] != m_old_keys_state[i]) 
							(*it)->onMouseLeftDown(Point(current_mouse_pos.x, current_mouse_pos.y));
					}
					else if (i == VK_RBUTTON)
					{
						if (m_keys_state[i] != m_old_keys_state[i]) 
							(*it)->onMouseRightDown(Point(current_mouse_pos.x, current_mouse_pos.y));
					}
					else (*it)->onKeyDown(i);
					++it;
				}
			}
			else //KEY IS RELEASED
			{
				if (m_keys_state[i] != m_old_keys_state[i])
				{
					std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();

					while (it != m_set_listeners.end())
					{
						
						if (i == MK_LBUTTON)
							(*it)->onMouseLeftUp(Point(current_mouse_pos.x, current_mouse_pos.y));
						else if (i == VK_RBUTTON)
							(*it)->onMouseRightUp(Point(current_mouse_pos.x, current_mouse_pos.y));
						else 
							(*it)->onKeyUp(i);
						++it;
					}
				}
			}
		}
		//store the keypress data in a second array
		std::memcpy(m_old_keys_state, m_keys_state, sizeof(unsigned char) * 256);
	}



}

void InputSystem::addListener(InputListener* il)
{
	m_set_listeners.insert(il);
}

void InputSystem::removeListener(InputListener* il)
{
	m_set_listeners.erase(il);
}

void InputSystem::setCursorPosition(const Point& pos)
{
	::SetCursorPos(pos.m_x, pos.m_y);
}

void InputSystem::showCursor(bool show)
{
	::ShowCursor(show);
}

InputSystem* InputSystem::get()
{
	return m_system;
}

void InputSystem::create()
{
	if (InputSystem::m_system) throw std::exception("InputSystem was already created");
	InputSystem::m_system = new InputSystem();
}

void InputSystem::release()
{
	if (!InputSystem::m_system) return;
	delete InputSystem::m_system;
}
