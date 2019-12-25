#include "Hourglass.h"
#include "SharedData.h"
#include "SoundManager.h"

std::shared_ptr<Hourglass> Hourglass::createInstance(const json &jsonData)
{
	return std::make_shared<Hourglass>();
}

Hourglass::Hourglass() : Pickup()
{
	// Load mesh and texture
	IAnimatedMesh* mesh = smgr->getMesh("models/hourglass.x");
	ITexture* texture = driver->getTexture("textures/hourglass.png");
	ITexture* normalMap = driver->getTexture("textures/hourglass_nm.png");

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->addTexture(1, normalMap);
	model->material = getCommonBasicMaterial(EMT_TRANSPARENT_VERTEX_ALPHA);
	models.push_back(model);
	model->normalMapping.textureIndex = 1;
	model->normalMapping.lightPower = 0.5f;
}

void Hourglass::update()
{
	Pickup::update();
}

void Hourglass::draw()
{
	Pickup::draw();

	// Draw model with behaviour
	if (notPicked)
	{
		std::shared_ptr<Model> &model = models.at(0);
		model->position = position;
		model->rotation += vector3df(0.0625f, 0.125f, 0.25f) * deltaTime;
	}
}

bool Hourglass::pick()
{
	if (Pickup::pick())
	{
		return true;
	}

	// Increment items picked counter
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_ITEMS_PICKED, 1);

	// Invert time
	SharedData::singleton->invertTime();

	// Set audio to play
	soundIndex = KEY_SOUND_HOURGLASS;

	return false;
}