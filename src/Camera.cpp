#include "Camera.h"

std::shared_ptr<Camera> Camera::singleton = nullptr;

Camera::Camera()
{
	position = vector3df(0, 0, 0);
	lookAt = vector3df(0, 0, 0);
}

void Camera::updateProperties()
{
	ICameraSceneNode* node = smgr->getActiveCamera();
	if (node != nullptr)
	{
		node->setPosition(position);
		node->setTarget(lookAt);
	}
}

vector3df Camera::getPosition()
{
	return position;
}

void Camera::setPosition(const vector3df & position)
{
	this->position = position;
	updateProperties();
}

vector3df Camera::getLookAt()
{
	return lookAt;
}

void Camera::setLookAt(const vector3df & lookAt)
{
	this->lookAt = lookAt;
	updateProperties();
}