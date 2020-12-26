#include "EventManager.h"

// Singleton initial value
std::shared_ptr<EventManager> EventManager::singleton = nullptr;

EventManager::EventManager()
{
	// Initialize window size
	windowSize = dimension2du(0, 0);

	// Initialize key states
	for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
	{
		keyStates[i] = KEY_NOT_PRESSED;
	}

	// Initialize data for mouse
	mousePosition = vector2di(0);
	mouseWheel = 0.0f;
}

bool EventManager::OnEvent(const SEvent& event)
{
	// Check if event is a Key Input Event
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
		if (event.KeyInput.PressedDown)
		{
			if (keyStates[event.KeyInput.Key] == KEY_NOT_PRESSED)
			{
				keyStates[event.KeyInput.Key] = KEY_PRESSED;
			}
		}
		else if (keyStates[event.KeyInput.Key] > KEY_NOT_PRESSED)
		{
			keyStates[event.KeyInput.Key] = KEY_RELEASED;
		}
	}
	// Check if event is a Mouse Event
	else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
	{
		switch (event.MouseInput.Event)
		{
		case EMIE_MOUSE_WHEEL:
			mouseWheel = event.MouseInput.Wheel;
			break;

		case EMIE_MOUSE_MOVED:
			mousePosition.X = event.MouseInput.X;
			mousePosition.Y = event.MouseInput.Y;
			break;

		case EMIE_LMOUSE_PRESSED_DOWN:
			if (keyStates[KEY_LBUTTON] == KEY_NOT_PRESSED)
			{
				keyStates[KEY_LBUTTON] = KEY_PRESSED;
			}
			break;

		case EMIE_MMOUSE_PRESSED_DOWN:
			if (keyStates[KEY_MBUTTON] == KEY_NOT_PRESSED)
			{
				keyStates[KEY_MBUTTON] = KEY_PRESSED;
			}
			break;

		case EMIE_RMOUSE_PRESSED_DOWN:
			if (keyStates[KEY_RBUTTON] == KEY_NOT_PRESSED)
			{
				keyStates[KEY_RBUTTON] = KEY_PRESSED;
			}
			break;

		case EMIE_LMOUSE_LEFT_UP:
			if (keyStates[KEY_LBUTTON] > KEY_NOT_PRESSED)
			{
				keyStates[KEY_LBUTTON] = KEY_RELEASED;
			}
			break;

		case EMIE_MMOUSE_LEFT_UP:
			if (keyStates[KEY_MBUTTON] > KEY_NOT_PRESSED)
			{
				keyStates[KEY_MBUTTON] = KEY_RELEASED;
			}
			break;

		case EMIE_RMOUSE_LEFT_UP:
			if (keyStates[KEY_RBUTTON] > KEY_NOT_PRESSED)
			{
				keyStates[KEY_RBUTTON] = KEY_RELEASED;
			}
			break;
		}
	}

	// Let Irrlicht process further events
	return false;
}

void EventManager::updateKeyStates()
{
	// Reset mouse wheel
	mouseWheel = 0;

	// Cycle through all available keys
	for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
	{
		// If key was just pressed
		if (keyStates[i] == KEY_PRESSED)
		{
			keyStates[i] = KEY_PRESSING;
		}

		// If key was just released
		else if (keyStates[i] == KEY_RELEASED)
		{
			keyStates[i] = KEY_NOT_PRESSED;
		}
	}
}