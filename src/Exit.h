#ifndef EXIT_H
#define EXIT_H

#include "GameObject.h"

class Exit : public GameObject
{
protected:
	f32 angle;

public:

	// Constructor
	Exit();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static shared_ptr<Exit> createInstance(const json &jsonData);
};

#endif // EXIT_H