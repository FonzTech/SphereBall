#include "Key.h"
#include "SharedData.h"
#include "SoundManager.h"

std::shared_ptr<Key> Key::createInstance(const json &jsonData)
{
	return std::make_shared<Key>();
}

Key::Key() : Pickup()
{
	// Load mesh and texture
	IAnimatedMesh* mesh = smgr->getMesh("models/key.obj");
	ITexture* texture = driver->getTexture("textures/key.png");

	// Load model
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->scale = vector3df(1, 1, 1);
	model->material = COMMON_BASIC_MATERIAL_SOLID;
	models.push_back(model);

	// Load sounds
	sounds[KEY_SOUND_KEY] = SoundManager::singleton->getSound(KEY_SOUND_KEY);
	sounds[KEY_SOUND_KEY_FINAL] = SoundManager::singleton->getSound(KEY_SOUND_KEY_FINAL);
}

void Key::update()
{
	Pickup::update();
}

void Key::draw()
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

bool Key::pick()
{
	if (Pickup::pick())
	{
		return true;
	}

	// Increment score
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_POINTS, 100);

	// Increment coin counter by one
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_KEY_PICKED, 1);

	// Increment items picked counter
	SharedData::singleton->updateGameScoreValue(KEY_SCORE_ITEMS_PICKED, 1);

	// Set sound to play
	soundIndex = SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_PICKED) >= SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_TOTAL) ? KEY_SOUND_KEY_FINAL : KEY_SOUND_KEY;

	return false;
}