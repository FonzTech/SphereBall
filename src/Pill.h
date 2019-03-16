#ifndef PILL_H
#define PILL_H

#include "GameObject.h"

class Pill : public GameObject
{
protected:

	/*
		Recognize the pill type.
		0 - Lethargy, 1 - Always Jump
	*/
	u8 type = 0;

public:
	// Constructor
	Pill();
	Pill(u8 type);

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static shared_ptr<Pill> createInstance(const json &jsonData);
};

#endif // PILL_H