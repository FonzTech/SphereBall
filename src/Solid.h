#ifndef SOLID_H
#define SOLID_H

#include "GameObject.h"
#include "ShaderCallback.h"

class Solid : public GameObject
{
protected:
	static const f32 BREAKING_THRESHOLD;

	aabbox3df boundingBox;

	f32 breakState;

	f32 springTension;
	f32 springAngle;

	s8 invisibleToggle;

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
		@param invisibleToggle give 0 to make block invisible when near to the player. Give 1 to
								make the block invisible when far from the player.
	*/
	Solid(const f32 breakState = -1.0f, const f32 springTension = -1.0f, const s8 invisibleToggle = -1);

	// Mandatory methods
	void update();
	void draw();
	aabbox3df getBoundingBox();

	// Create specialized instance
	static std::shared_ptr<Solid> createInstance(const json &jsonData);
	
	// Behaviour
	bool isSolid();

	// ShaderCallBack
	class SpecializedShaderCallback : public ShaderCallback
	{
	protected:
		Solid* solid;

	public:
		SpecializedShaderCallback(Solid* solid);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	};
};

#endif // SOLID_H