#include "Coin.h"
#include "SharedData.h"
#include "SoundManager.h"

std::shared_ptr<Coin> Coin::createInstance(const json &jsonData)
{
	return std::make_shared<Coin>();
}

Coin::Coin() : Pickup()
{
	// Load mesh and texture
	IAnimatedMesh* mesh = smgr->getMesh("models/coin.obj");
	ITexture* texture = driver->getTexture("textures/coin.png");

	// Load model
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->scale = vector3df(1, 1, 1);
	models.push_back(model);
}

void Coin::update()
{
	Pickup::update();
}

void Coin::draw()
{
	Pickup::draw();

	// Draw model with behaviour
	if (notPicked)
	{
		std::shared_ptr<Model> &model = models.at(0);
		model->position = position;
		model->rotation = vector3df(0, angle, 0);
	}
}

bool Coin::pick()
{
	if (Pickup::pick())
	{
		return true;
	}

	// Increment score
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_POINTS, 50);

	// Increment coin counter by one
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_COIN, 1);

	// Increment items picked counter
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_ITEMS_PICKED, 1);

	// Set sound to play
	soundIndex = KEY_SOUND_COIN;

	return false;
}