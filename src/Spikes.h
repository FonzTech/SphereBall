#ifndef SPIKES_H
#define SPIKES_H

#include "GameObject.h"
#include <SFML/Audio.hpp>
#include "Alarm.h"

using namespace std;

class Spikes : public GameObject
{
protected:
	static const f32 TIP_HEIGHT;

	unique_ptr<Alarm> timer;
	s8 mode;
	f32 tipY;

public:
	// Constructor
	Spikes();
	Spikes(s8 initialMode, f32 delay);

	// Create specialized instance
	static shared_ptr<Spikes> createInstance(const json &jsonData);

	// Mandatory methods
	void update();
	void draw();

	// Speicalized methods
	s8 isHarmful();
};

#endif // SKYBOX_H