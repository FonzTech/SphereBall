#include "Key.h"
#include "SharedData.h"

std::shared_ptr<Key> Key::createInstance(const json &jsonData)
{
	return std::make_shared<Key>();
}

Key::Key() : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh = smgr->getMesh("models/key.obj");

	// Load texture
	ITexture* texture = driver->getTexture("textures/key.png");

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);

	// Initialize variables
	angle = 0;
}

void Key::update()
{
	angle += 0.25f * deltaTime;
}

void Key::draw()
{
	// Update model
	std::shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->rotation = vector3df(0, angle, 0);
}

bool Key::pick()
{
	// Increment score
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_POINTS, 100);

	// Increment coin counter by one
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_KEY_PICKED, 1);

	// Increment items picked counter
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_ITEMS_PICKED, 1);

	// Mark item to be destroyed
	destroy = true;

	// Check if all of the keys have been picked
	if (SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_PICKED) >= SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_TOTAL))
	{
		return true;
	}

	return false;
}