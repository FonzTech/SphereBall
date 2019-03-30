#include "Camera.h"

std::shared_ptr<Camera> Camera::singleton = nullptr;

Camera::Camera()
{
	position = vector3df(0, 0, 0);
	lookAt = vector3df(0, 0, 0);
}