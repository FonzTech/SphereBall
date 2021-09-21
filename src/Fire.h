#ifndef FIRE_H
#define FIRE_H

#include "GameObject.h"

class Fire : public GameObject
{
protected:

	IParticleSystemSceneNode* particleSystem;

	// Behaviour
	void createFileParticle();

public:

	// Constructor and deconstructor
	Fire();
	~Fire();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<Fire> createInstance(const nlohmann::json &jsonData);
};

#endif // HOURGLASS_H