#ifndef KEY_H
#define KEY_H

#include "GameObject.h"

class Key : public GameObject
{
protected:

	f32 angle;

public:
	// Constructor
	Key();

	// Mandatory methods
	void update();
	void draw();

	/*
		Use this method to trigger item picking.
		It returns true is all of the keys have been picked, false otherwise.
	*/
	bool pick();

	// Create specialized instance
	static std::shared_ptr<Key> createInstance(const json &jsonData);
};

#endif // KEY_H