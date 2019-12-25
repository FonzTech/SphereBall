#include "Coin.h"
#include "SharedData.h"
#include "SoundManager.h"

std::shared_ptr<Coin> Coin::createInstance(const json &jsonData)
{
	u8 type = 0;
	try
	{
		json optional = jsonData.at("optional");
		optional.at("type").get_to(type);
	}
	catch (json::exception e)
	{
	}
	return std::make_shared<Coin>(type);
}

Coin::Coin(const u8 type) : Pickup()
{
	// Assign type
	this->type = type;

	// Get correct texture
	std::string textureFile = std::string("textures/") + (type == 1 ? "coin_blue" : "coin");

	// Load mesh and texture
	IAnimatedMesh* mesh = smgr->getMesh("models/coin.obj");
	ITexture* texture = driver->getTexture((textureFile + ".png").c_str());
	ITexture* normalMap = driver->getTexture((textureFile + "_nm.png").c_str());

	// Load model
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->addTexture(1, normalMap);
	model->scale = vector3df(1, 1, 1);
	model->material = getCommonBasicMaterial(EMT_SOLID);
	model->normalMapping.textureIndex = 1;
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
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_POINTS, type == 1 ? 100 : 50);

	// Increment coin counter by one
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_COIN, 1);

	// Increment items picked counter
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_ITEMS_PICKED, 1);

	// Set sound to play
	soundIndex = KEY_SOUND_COIN;

	return false;
}