#ifndef COLLISION_H
#define COLLISION_H

#include <memory>
#include "EngineObject.h"

using namespace std;

using namespace irr;
using namespace core;

// Structure which acts as a container to hold collision results
class Collision
{
public:

	shared_ptr<EngineObject> gameObject = nullptr;
	aabbox3df mainBoundingBox, otherBoundingBox;

	// Get casted game object which collided
	template <typename T>
	shared_ptr<T> getGameObject()
	{
		return static_pointer_cast<T>(gameObject);
	}
};

#endif // COLLISION_H