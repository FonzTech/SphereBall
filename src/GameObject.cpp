#include "GameObject.h"

shared_ptr<GameObject> GameObject::createInstance(const json &jsonData)
{
	return nullptr;
}

GameObject::GameObject()
{
	// Initialize vector for models
	models = vector<shared_ptr<Model>>();

	// Initialize variables
	destroy = false;
}