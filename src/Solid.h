#ifndef SOLID_H
#define SOLID_H

#include "GameObject.h"

class Solid : public GameObject
{
public:
	// Constructor
	Solid();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<Solid> createInstance(const json &jsonData);
};

#endif // SOLID_H