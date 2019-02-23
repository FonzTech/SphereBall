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
	sounds["spike_in"] = SoundManager::singleton->getSound("spike_in");
	sounds["spike_out"] = SoundManager::singleton->getSound("spike_out");

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
		if (mode)
		{
			mode = 0;
			sounds["spike_out"]->play();
			timer->setTime(1500);
		}
		else
		{
			mode = 1;
			sounds["spike_in"]->play();
			timer->setTime(2250);
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