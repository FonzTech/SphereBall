#ifndef CAMERA_H
#define CAMERA_H

#include <memory>
#include <irrlicht.h>

using namespace irr;
using namespace core;

using namespace std;

class Camera
{
public:
	// Singleton holder
	static shared_ptr<Camera> singleton;

	// Constructor
	Camera();

	// Camera properties
	vector3df position;
	vector3df lookAt;
};

#endif // CAMERA_H