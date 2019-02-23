#include "Solid.h"
#include "EventManager.h"

shared_ptr<Solid> Solid::createInstance(const json &jsonData)
{
	return make_shared<Solid>();
}

Solid::Solid() : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh = smgr->getMesh("models/cube.obj");

	// Load texture
	ITexture* texture = driver->getTexture("textures/block.png");

	// Create model for player
	shared_ptr<Model> model = make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);
}

void Solid::update()
{
}

void Solid::draw()
{
	// Update model
	shared_ptr<Model> model = models.at(0);
	model->position = position;
}