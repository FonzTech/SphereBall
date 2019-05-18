#ifndef SKYBOX_H
#define SKYBOX_H

#include <SFML/Audio.hpp>
#include <string>

#include "GameObject.h"

class SkyBox : public GameObject
{
protected:
	static const std::string FRAMES[];
	static const f32 DEFAULT_MODEL_ANGLE;
	static const f32 FACE_DISTANCE;
	static const f32 FACE_BIAS;

	float angle;

public:
	// Constructor
	SkyBox(const std::string &textureName);

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<SkyBox> createInstance(const json &jsonData);
};

#endif // SKYBOX_H