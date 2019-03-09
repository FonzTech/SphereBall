#include "Spikes.h"
#include "SoundManager.h"

const f32 Spikes::TIP_HEIGHT = 10;

shared_ptr<Spikes> Spikes::createInstance(const json &jsonData)
{
	u32 initialMode = -1;
	f32 delay;
	try
	{
		json optional = jsonData.at("optional");
		optional.at("mode").get_to(initialMode);

		try
		{
			optional.at("delay").get_to(delay);
		}
		catch (json::exception ex)
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
	catch (json::exception e)
	{
		if (initialMode == -1)
		{
			initialMode = 1;
			delay = 2500;
		}
	}
	return make_shared<Spikes>(initialMode, delay);
}

Spikes::Spikes() : Spikes(1, 2500)
{
}

Spikes::Spikes(s8 initialMode, f32 delay) : GameObject()
{
	// Load mesh
	IAnimatedMesh* meshA = smgr->getMesh("models/spikes_base.obj");
	IAnimatedMesh* meshB = smgr->getMesh("models/spikes_tip.obj");

	// Load texture
	ITexture* texture = driver->getTexture("textures/spikes.png");

	// Create model for base
	shared_ptr<Model> model = make_shared<Model>(meshA);
	model->addTexture(0, texture);
	models.push_back(model);

	// Create model for tip
	model = make_shared<Model>(meshB);
	model->addTexture(0, texture);
	model->scale = vector3df(1, 1, 1);
	models.push_back(model);

	// Load sounds
	sounds[KEY_SOUND_SPIKE_IN] = SoundManager::singleton->getSound(KEY_SOUND_SPIKE_IN);
	sounds[KEY_SOUND_SPIKE_IN]->setMinDistance(50.0f);
	sounds[KEY_SOUND_SPIKE_IN]->setAttenuation(25.0f);

	sounds[KEY_SOUND_SPIKE_OUT] = SoundManager::singleton->getSound(KEY_SOUND_SPIKE_OUT);
	sounds[KEY_SOUND_SPIKE_OUT]->setMinDistance(50.0f);
	sounds[KEY_SOUND_SPIKE_OUT]->setAttenuation(25.0f);

	// Initialize alarms
	timer = make_unique<Alarm>(delay);

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

void Spikes::update()
{
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
			playSound(KEY_SOUND_SPIKE_OUT, &position);
		}
		else
		{
			// Trigger after 2.250 seconds
			mode = 1;
			timer->setTime(2250);

			// Play spatial sound
			playSound(KEY_SOUND_SPIKE_IN, &position);
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
	shared_ptr<Model> model = models.at(0);
	model->position = position;

	model = models.at(1);
	model->position = position + vector3df(0, 3 - TIP_HEIGHT * tipY, 0);
}

s8 Spikes::isHarmful()
{
	return tipY < 0.5f;
}