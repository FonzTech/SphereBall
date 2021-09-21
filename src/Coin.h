#ifndef COIN_H
#define COIN_H

#include "Pickup.h"

class Coin : public Pickup
{
protected:

	// Coin type - 0 = Yellow, 1 = Blue
	u8 type;

public:

	// Constructor
	Coin(const u8 type);

	// Mandatory methods
	void update();
	void draw();

	bool pick();

	// Create specialized instance
	static std::shared_ptr<Coin> createInstance(const nlohmann::json &jsonData);
};

#endif // COIN_H