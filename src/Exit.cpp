#include <algorithm>

#include "Exit.h"
#include "SharedData.h"

shared_ptr<Exit> Exit::createInstance(const json &jsonData)
{
	return make_shared<Exit>();
}

Exit::Exit() : GameObject()
{
	// Initialize variables
	angle = 0;
	picked = 0;

	// Load mesh and texture for Exit model
	IAnimatedMesh* mesh = smgr->getMesh("models/exit.obj");
	ITexture* texture = driver->getTexture("textures/exit_red.png");
	
	// Create model for player
	shared_ptr<Model> model = make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = EMT_TRANSPARENT_ALPHA_CHANNEL;
	models.push_back(model);

	// Load mesh and texture for Base model
	mesh = smgr->getMesh("models/plane.obj");
	texture = driver->getTexture("textures/exit_base_red.png");

	// Create model for base
	model = make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->rotation = vector3df(90, 0, 0);
	model->scale = vector3df(1, 1, 1);
	models.push_back(model);
}

void Exit::update()
{
	if (SharedData::singleton->getGameScoreValue(KEY_GUI_KEY_PICKED) >= SharedData::singleton->getGameScoreValue(KEY_GUI_KEY))
	{
		if (picked)
		{
			angle += 0.25f * deltaTime;
		}
		else
		{
			pick();
		}
	}
	else
	{
		angle += 0.1f * deltaTime;
	}
}

void Exit::draw()
{
	// Exit model
	shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->rotation = vector3df(0, angle, 0);

	// Base model
	model = models.at(1);
	model->position = position + vector3df(0, -9.7f, 0);
}

void Exit::pick()
{
	// Mark item as picked
	picked = 1;

	// Replace textures
	models.at(0)->textures[0] = driver->getTexture("textures/exit_green.png");
	models.at(1)->textures[0] = driver->getTexture("textures/exit_base_green.png");
}