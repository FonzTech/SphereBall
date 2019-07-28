#include "Fruit.h"
#include "SharedData.h"

std::shared_ptr<Fruit> Fruit::createInstance(const json &jsonData)
{
	return std::make_shared<Fruit>();
}

Fruit::Fruit() : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh = smgr->getMesh("models/apple.obj");

	// Load texture
	ITexture* texture = driver->getTexture("textures/apple.png");

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);

	// Initialize variables
	angle = 0;
	floatEffect = 0;
}

void Fruit::update()
{
	angle += 0.1f * deltaTime;
	floatEffect += 0.05f;
}

void Fruit::draw()
{
	// Update model
	f32 fx = std::sin(floatEffect);
	f32 topup = std::sin(floatEffect * 0.75f) * 0.5f + 0.5f;

	std::shared_ptr<Model> model = models.at(0);
	model->position = position + vector3df(0, fx, 0);
	model->rotation = vector3df(topup * 10.0f, angle, 0);
}

void Fruit::pick()
{
	// Increment score
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_POINTS, 1000);

	// Increment fruit counter by one
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_FRUITS, 1);

	// Mark item to be destroyed
	destroy = true;
}