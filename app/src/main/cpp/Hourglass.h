#ifndef HOURGLASS_H
#define HOURGLASS_H

#include "Pickup.h"

class Hourglass : public Pickup
{
public:

	// Constructor
	Hourglass();

	// Mandatory methods
	void update();
	void draw();

	bool pick();

	// Create specialized instance
	static std::shared_ptr<Hourglass> createInstance(const json &jsonData);
};

#endif // HOURGLASS_H