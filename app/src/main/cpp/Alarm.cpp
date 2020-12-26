#include "Alarm.h"

Alarm::Alarm(f32 initialTime)
{
	currentTime = initialTime;
}

void Alarm::setTime(f32 time)
{
	currentTime = time;
}

void Alarm::stepDecrement(f32 deltaTime)
{
	currentTime -= deltaTime;
}

bool Alarm::isTriggered()
{
	return currentTime < 0;
}