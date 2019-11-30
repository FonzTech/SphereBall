#ifndef TELEPORTER_H
#define TELEPORTER_H

#include "GameObject.h"
#include "EventManager.h"

class Teleporter : public GameObject
{
protected:

	static const std::string VECTOR_COMPONENTS[];
	static const std::string COLOR_COMPONENTS[];

	// Object behaviour
	s32 customMaterial;
	f32 angle;
	SColorf color;

	// ShaderCallBack
	class SpecializedShaderCallback : public ShaderCallback
	{
	protected:
		Teleporter* teleporter;

	public:
		SpecializedShaderCallback(Teleporter* teleporter);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	};

public:

	// Constructor
	Teleporter(const vector3df & warp, const SColorf & color);

	// Warp position
	vector3df warp;

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<Teleporter> createInstance(const json &jsonData);
};

#endif // TELEPORTER_H