#ifndef HOURGLASS_H
#define HOURGLASS_H

#include "GameObject.h"
#include "EventManager.h"
#include "ShaderCallback.h"
#include <SFML/Audio.hpp>

class Hourglass : public GameObject
{
protected:

	// Plane model
	std::shared_ptr<Model> planeModel;

public:

	// Constructor
	Hourglass();

	// Mandatory methods
	void update();
	void draw();

	// Specialized methods
	void pick();

	// Create specialized instance
	static std::shared_ptr<Hourglass> createInstance(const json &jsonData);
};

#endif // HOURGLASS_H