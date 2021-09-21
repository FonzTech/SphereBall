#include "Spikes.h"
#include "SoundManager.h"

using nlohmann::json;

const f32 Spikes::TIP_HEIGHT = 10;

std::shared_ptr<Spikes> Spikes::createInstance(const nlohmann::json &jsonData)
{
	u32 initialMode = -1;
	f32 delay;
	try
	{
		nlohmann::json optional = jsonData.at("optional");
		optional.at("mode").get_to(initialMode);

		if (initialMode == -1)
		{
			delay = -1;
		}
		else
		{
			try
			{
				optional.at("delay").get_to(delay);
			}
			catch (nlohmann::json::exception ex)
			{
				if (initialMode)
				{
					delay = 2250;
				}
				else
				{
					delay = 1500;
				}
			}
		}
	}
	catch (nlohmann::json::exception e)
	{
		if (initialMode == -1)
		{
			initialMode = 1;
			delay = 2500;
		}
	}
	return std::make_shared<Spikes>(initialMode, delay);
}

Spikes::Spikes() : Spikes(1, 2500)
{
}

Spikes::Spikes(s8 initialMode, f32 delay) : GameObject()
{
	// Check for mode
	if (delay < 0)
	{
		// Load mesh
		IAnimatedMesh* mesh = smgr->getMesh("models/spikes_b.x");

		// Load texture
		ITexture* texture = driver->getTexture("textures/spikes_b.png");
		ITexture* normalMap = driver->getTexture("textures/spikes_b_nm.png");

		// Create model for base
		std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
		model->addTexture(0, texture);
		model->addTexture(1, normalMap);
		model->material = getCommonBasicMaterial(EMT_SOLID);
		model->normalMapping.textureIndex = 1;
		models.push_back(model);
	}
	else
	{
		// Load mesh
		IAnimatedMesh* meshA = smgr->getMesh("models/spikes_base.x");
		IAnimatedMesh* meshB = smgr->getMesh("models/spikes_tip.x");

		// Load texture
		ITexture* texture = driver->getTexture("textures/spikes.png");
		ITexture* normalMap = driver->getTexture("textures/spikes_nm.png");

		// Create model for base
		std::shared_ptr<Model> model = std::make_shared<Model>(meshA);
		model->addTexture(0, texture);
		model->addTexture(1, normalMap);
		model->material = getCommonBasicMaterial(EMT_SOLID);
		model->normalMapping.textureIndex = 1;
		models.push_back(model);

		// Create model for tip
		model = std::make_shared<Model>(meshB);
		model->addTexture(0, texture);
		model->scale = vector3df(1, 1, 1);
		model->addTexture(1, normalMap);
		model->material = getCommonBasicMaterial(EMT_SOLID);
		model->normalMapping.textureIndex = 1;
		models.push_back(model);

		// Load sounds
		sounds[KEY_SOUND_SPIKE_IN] = SoundManager::singleton->getSound(KEY_SOUND_SPIKE_IN);
		sounds[KEY_SOUND_SPIKE_IN]->setMinDistance(50.0f);
		sounds[KEY_SOUND_SPIKE_IN]->setAttenuation(25.0f);

		sounds[KEY_SOUND_SPIKE_OUT] = SoundManager::singleton->getSound(KEY_SOUND_SPIKE_OUT);
		sounds[KEY_SOUND_SPIKE_OUT]->setMinDistance(50.0f);
		sounds[KEY_SOUND_SPIKE_OUT]->setAttenuation(25.0f);

		// Initialize alarms
		timer = std::make_unique<Alarm>(delay);

		// Initialize variables
		mode = initialMode;
		if (mode)
		{
			tipY = 1;
		}
		else
		{
			tipY = 0;
		}
	}
}

void Spikes::update()
{
	// Check for timer management
	if (timer == nullptr)
	{
		return;
	}

	// Control mode switching
	timer->stepDecrement(deltaTime);
	if (timer->isTriggered())
	{
		// Check for mode
		if (mode)
		{
			// Trigger after 1.500 seconds
			mode = 0;
			timer->setTime(1500);

			// Play spatial sound
			playAudio(KEY_SOUND_SPIKE_OUT, &position);
		}
		else
		{
			// Trigger after 2.250 seconds
			mode = 1;
			timer->setTime(2250);

			// Play spatial sound
			playAudio(KEY_SOUND_SPIKE_IN, &position);
		}
	}

	// Control position for tip model
	if (mode)
	{
		tipY += deltaTime * 0.0015f;
		if (tipY > 1)
		{
			tipY = 1;
		}
	}
	else
	{
		tipY -= deltaTime * 0.01f;
		if (tipY < 0)
		{
			tipY = 0;
		}
	}
}

void Spikes::draw()
{
	// Update model
	std::shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->scale = vector3df(1);

	if (models.size() > 1)
	{
		model = models.at(1);
		model->position = position + vector3df(0, 3 - TIP_HEIGHT * tipY, 0);
	}
}

s8 Spikes::isHarmful()
{
	return mode < 0 || tipY < 0.5f;
}