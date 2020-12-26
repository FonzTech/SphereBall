#ifndef PILL_H
#define PILL_H

#include "Pickup.h"

class Pill : public Pickup
{
protected:

	/*
		Recognize the pill type.
		0 - Lethargy, 1 - Always Jump
	*/
	u8 type = 0;

	// Fake light shader
	s32 customMaterial;

public:

	// Constructor
	Pill();
	Pill(u8 type);

	// Mandatory methods
	void update();
	void draw();

	bool pick();

	// Create specialized instance
	static std::shared_ptr<Pill> createInstance(const json &jsonData);

	// ShaderCallBack
	class PillShaderCallback : public ShaderCallback
	{
	protected:
		Pill* pill;

	public:
		PillShaderCallback(Pill* pill);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	};
};

#endif // PILL_H