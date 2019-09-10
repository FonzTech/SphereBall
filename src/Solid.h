#ifndef SOLID_H
#define SOLID_H

#include "GameObject.h"

class Solid : public GameObject
{
protected:
	static const f32 BREAKING_THRESHOLD;

	aabbox3df boundingBox;

	f32 breakState;

	f32 springTension;
	f32 springAngle;

public:
	/*
		Constructor for Solid class. Don't pass any parameter (or pass the default ones) to
		make this object a static and plain solid block. You can turn this into:
			- breakable block with a starting state.
			- spring block with a pre-defined "spring tension", where the greater this factor
			  is, the taller is the jump given to the player.

		@param breakState the break state for the block, which ranges in [ 0, BREAKING_THRESHOLD ]
							interval. Smaller values aren't taken into account. Greater values will
							make the block break immediately in game.
		@param springTension a multiplier describing how tall the jump given to the player must be.
								Values smaller than zero aren't taken into account. Greater values
								than 1 will give an overpowered jump, but it's still legal.
	*/
	Solid(const f32 breakState = -1.0f, const f32 springTension = -1.0f);

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