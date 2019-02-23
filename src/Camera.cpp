#include "Camera.h"

shared_ptr<Camera> Camera::singleton = nullptr;

Camera::Camera()
{
	position = lookAt = vector3df(0, 0, 0);
}