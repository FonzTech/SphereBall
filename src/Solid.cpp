#include "Solid.h"
#include "EventManager.h"

std::shared_ptr<Solid> Solid::createInstance(const json &jsonData)
{
	return std::make_shared<Solid>();
}

Solid::Solid() : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh = smgr->getMesh("models/cube.obj");

	// Load texture
	ITexture* texture = driver->getTexture("textures/block.png");

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);
}

void Solid::update()
{
}

void Solid::draw()
{
	// Update model
	std::shared_ptr<Model> model = models.at(0);
	model->position = position;
}