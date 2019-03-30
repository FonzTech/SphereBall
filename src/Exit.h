#ifndef EXIT_H
#define EXIT_H

#include "GameObject.h"

class Exit : public GameObject
{
protected:
	f32 angle;
	s8 picked;

public:

	// Constructor
	Exit();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<Exit> createInstance(const json &jsonData);

	// Method to change this object to "picked" state
	void pick();
};

#endif // EXIT_H