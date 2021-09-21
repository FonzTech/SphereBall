#ifndef SKYBOX_H
#define SKYBOX_H

#include <SFML/Audio.hpp>
#include <string>

#include "GameObject.h"

class SkyBox : public GameObject
{
protected:
	static const std::string FRAMES[];

	float angle;

public:
	// Constructor
	SkyBox(const std::string &textureName);

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<SkyBox> createInstance(const nlohmann::json &jsonData);
};

#endif // SKYBOX_H