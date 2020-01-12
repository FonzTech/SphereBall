#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <memory>
#include <irrlicht.h>

#define KEY_NOT_PRESSED 0
#define KEY_PRESSED 1
#define KEY_PRESSING 2
#define KEY_RELEASED -1

using namespace irr;
using namespace core;

class EventManager : public IEventReceiver
{
public:
	// Singleton pattern variable
	static std::shared_ptr<EventManager> singleton;

	// Constructor
	EventManager();

	// We use this data structore to store current window size
	dimension2du windowSize;

	// We use this array to store the current state of each key
	s8 keyStates[KEY_KEY_CODES_COUNT];

	// We use this data structure to store current mouse position
	vector2di mousePosition;

	// Variable for mouse wheel state
	f32 mouseWheel;

	// OnEvent callback
	bool OnEvent(const SEvent& event);

	// Update key states
	void updateKeyStates();
};

#endif // INPUTMANAGER_H