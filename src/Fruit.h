#ifndef FRUIT_H
#define FRUIT_H

#include "Pickup.h"

class Fruit : public Pickup
{
protected:

	f32 floatEffect;

public:
	// Constructor
	Fruit();

	// Mandatory methods
	void update();
	void draw();

	bool pick();

	// Create specialized instance
	static std::shared_ptr<Fruit> createInstance(const json &jsonData);
};

#endif // FRUIT_H