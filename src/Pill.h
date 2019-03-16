#ifndef PILL_H
#define PILL_H

#include "GameObject.h"
#include "ShaderCallBack.h"

class Pill : public GameObject
{
protected:

	/*
		Recognize the pill type.
		0 - Lethargy, 1 - Always Jump
	*/
	u8 type = 0;

	// Fake light shader
	static s32 customMaterial;

public:
	// Constructor
	Pill();
	Pill(u8 type);

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static shared_ptr<Pill> createInstance(const json &jsonData);

	// ShaderCallBack
	class SpecializedShaderCallback : public ShaderCallBack
	{
	protected:
		Pill* pill;

	public:
		SpecializedShaderCallback(Pill* pill);
		virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData);
	};
};

#endif // PILL_H