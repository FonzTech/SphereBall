#ifndef CAMERA_H
#define CAMERA_H

#include <memory>
#include <irrlicht.h>

#include "EngineObject.h"

using namespace irr;
using namespace core;

class Camera : public EngineObject
{
protected:
	// Camera properties
	vector3df position;
	vector3df lookAt;

	void updateProperties();

public:
	// Singleton holder
	static std::shared_ptr<Camera> singleton;

	// Constructor
	Camera();

	// Camera properties
	vector3df getPosition();
	void setPosition(const vector3df & position);

	vector3df getLookAt();
	void setLookAt(const vector3df & lookAt);
};

#endif // CAMERA_H