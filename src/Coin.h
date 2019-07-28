#ifndef COIN_H
#define COIN_H

#include "Pickup.h"

class Coin : public Pickup
{
public:

	// Constructor
	Coin();

	// Mandatory methods
	void update();
	void draw();

	bool pick();

	// Create specialized instance
	static std::shared_ptr<Coin> createInstance(const json &jsonData);
};

#endif // COIN_H