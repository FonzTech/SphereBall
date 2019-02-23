#include "Key.h"
#include "SharedData.h"

shared_ptr<Key> Key::createInstance(const json &jsonData)
{
	return make_shared<Key>();
}

Key::Key() : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh = smgr->getMesh("models/key.obj");

	// Load texture
	ITexture* texture = driver->getTexture("textures/key.png");

	// Create model for player
	shared_ptr<Model> model = make_shared<Model>(mesh);
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
	shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->rotation = vector3df(0, angle, 0);
}

u8 Key::pick()
{
	// Increment coin counter by one
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_KEY_PICKED, 1);

	// Mark item to be destroyed
	destroy = true;

	// Check if all of the keys have been picked
	if (SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_PICKED) >= SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY))
	{
		return 1;
	}

	return 0;
}