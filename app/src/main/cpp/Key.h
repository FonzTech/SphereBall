#ifndef KEY_H
#define KEY_H

#include "Pickup.h"

class Key : public Pickup
{
public:

	// Constructor
	Key();

	// Mandatory methods
	void update();
	void draw();

	bool pick();

	// Create specialized instance
	static std::shared_ptr<Key> createInstance(const json &jsonData);
};

#endif // KEY_H