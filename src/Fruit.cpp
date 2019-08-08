#include <string>

#include "Fruit.h"
#include "SharedData.h"
#include "SoundManager.h"

std::shared_ptr<Fruit> Fruit::createInstance(const json &jsonData)
{
	return std::make_shared<Fruit>();
}

Fruit::Fruit() : Pickup()
{
	// Load mesh and model
	IAnimatedMesh* mesh;
	ITexture* texture;

	{
		std::string fruitToLoad = "apple";

		if (SharedData::singleton->getGameScoreValue(KEY_SCORE_FRUITS) == 1)
		{
			fruitToLoad = "banana";
		}

		mesh = smgr->getMesh(std::string("models/" + fruitToLoad + ".obj").c_str());
		texture = driver->getTexture(std::string("textures/" + fruitToLoad + ".png").c_str());
	}

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);

	// Initialize variables
	floatEffect = 0;
}

void Fruit::update()
{
	if (notPicked)
	{
		angle += 0.1f * deltaTime;
		floatEffect += 0.05f;
	}
}

void Fruit::draw()
{
	Pickup::draw();

	// Draw model with behaviour
	if (notPicked)
	{
		// Update model
		f32 fx = std::sin(floatEffect);
		f32 topup = std::sin(floatEffect * 0.75f) * 0.5f + 0.5f;

		std::shared_ptr<Model> model = models.at(0);
		model->position = position + vector3df(0, fx, 0);
		model->rotation = vector3df(topup * 10.0f, angle, 0);
	}
}

bool Fruit::pick()
{
	if (Pickup::pick())
	{
		return true;
	}

	// Increment score
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_POINTS, 1000);

	// Increment fruit counter by one
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_FRUITS, 1);

	// Set audio to play
	soundIndex = KEY_SOUND_FRUIT;

	return false;
}