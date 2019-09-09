#ifndef SOLID_H
#define SOLID_H

#include "GameObject.h"

class Solid : public GameObject
{
protected:
	static const f32 BREAKING_THRESHOLD;

	aabbox3df boundingBox;
	f32 breakState;

public:
	// Constructor
	Solid(const f32 breakState);

	// Mandatory methods
	void update();
	void draw();
	aabbox3df getBoundingBox();

	// Create specialized instance
	static std::shared_ptr<Solid> createInstance(const json &jsonData);
	
	// Behaviour
	bool isSolid();
};

#endif // SOLID_H