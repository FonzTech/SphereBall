#ifndef FRUIT_H
#define FRUIT_H

#include "GameObject.h"

class Fruit : public GameObject
{
protected:

	f32 angle, floatEffect;

public:
	// Constructor
	Fruit();

	// Mandatory methods
	void update();
	void draw();

	/*
		Use this method to trigger item picking.
	*/
	void pick();

	// Create specialized instance
	static std::shared_ptr<Fruit> createInstance(const json &jsonData);
};

#endif // FRUIT_H