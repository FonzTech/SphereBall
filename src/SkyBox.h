#ifndef SKYBOX_H
#define SKYBOX_H

#include "GameObject.h"
#include <SFML/Audio.hpp>
#include <string>

using namespace std;

class SkyBox : public GameObject
{
protected:
	float angle;

public:
	// Constructor
	SkyBox(std::string &textureFilename);

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static shared_ptr<SkyBox> createInstance(const json &jsonData);
};

#endif // SKYBOX_H