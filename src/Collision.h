#ifndef COLLISION_H
#define COLLISION_H

#include <memory>
#include "EngineObject.h"

using namespace irr;
using namespace core;

// Structure which acts as a container to hold collision results
class Collision
{
public:

	std::shared_ptr<EngineObject> engineObject = nullptr;
	aabbox3df mainBoundingBox, otherBoundingBox;

	// Get casted game object which collided
	template <typename T>
	std::shared_ptr<T> getGameObject()
	{
		return std::static_pointer_cast<T>(engineObject);
	}
};

#endif // COLLISION_H