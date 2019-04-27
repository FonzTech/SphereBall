#include "Hourglass.h"
#include "SharedData.h"

std::shared_ptr<Hourglass> Hourglass::createInstance(const json &jsonData)
{
	return std::make_shared<Hourglass>();
}

Hourglass::Hourglass() : GameObject()
{
	// Load mesh and texture
	IAnimatedMesh* mesh = smgr->getMesh("models/hourglass.obj");
	ITexture* texture = driver->getTexture("textures/hourglass.png");

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = EMT_TRANSPARENT_ALPHA_CHANNEL;
	models.push_back(model);
}

void Hourglass::update()
{
}

void Hourglass::draw()
{
	std::shared_ptr<Model> &model = models.at(0);
	model->position = position;
	model->rotation += vector3df(0.0625f, 0.125f, 0.25f) * deltaTime;
}

void Hourglass::pick()
{
	// Increment items picked counter
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_ITEMS_PICKED, 1);

	// Mark item as to be destroyed
	destroy = true;
}