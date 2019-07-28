#ifndef PICKUP_H
#define PICKUP_H

#include "GameObject.h"
#include "EventManager.h"
#include "ShaderCallback.h"

class Pickup : public GameObject
{
protected:
	f32 angle;

	// Plane model
	std::shared_ptr<Model> planeModel;

	// Sparkle shader
	s32 customShader;

public:

	// Specialized variables
	std::string soundIndex;
	bool notPicked;

	// Constructor
	Pickup();

	// Mandatory methods
	virtual void update();
	virtual void draw();

	// Specialized methods
	virtual bool pick();

	// ShaderCallBack
	class SpecializedShaderCallback : public ShaderCallback
	{
	protected:
		Pickup* pickup;

	public:
		SpecializedShaderCallback(Pickup* coin);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	};
};

#endif // PICKUP_H