#ifndef ALARM_H
#define ALARM_H

#include <irrlicht.h>

using namespace irr;
using namespace core;

class Alarm
{
protected:
	f32 currentTime;

public:
	// Consturctor
	Alarm(f32 initialTime);

	// Set time
	void setTime(f32 time);

	// Decrement by
	void stepDecrement(f32 deltaTime);

	// Is triggered
	bool isTriggered();
};

#endif // ALARM_H