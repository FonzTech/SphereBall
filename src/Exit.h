#ifndef EXIT_H
#define EXIT_H

#include "GameObject.h"
#include "ShaderCallback.h"

class Exit : public GameObject
{
protected:
	f32 angle;
	s8 picked;
	SColorf color;

	s32 customMaterial;

public:

	// Constructor
	Exit();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<Exit> createInstance(const json &jsonData);

	// Method to change this object to "picked" state
	void pick();
	
	// Method to fade object away
	void fade();

	// ShaderCallBack
	class SpecializedShaderCallback : public ShaderCallback
	{
	protected:
		Exit* exit;

	public:
		SpecializedShaderCallback(Exit* exit);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	};
};

#endif // EXIT_H