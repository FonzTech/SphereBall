#include "Pill.h"

shared_ptr<Pill> Pill::createInstance(const json &jsonData)
{
	u8 type;
	try
	{
		json optional = jsonData.at("optional");
		optional.at("type").get_to(type);
	}
	catch (json::exception e)
	{
		type = 0;
	}
	return make_shared<Pill>(type);
}

Pill::Pill() : Pill(0)
{
}

Pill::Pill(u8 type) : GameObject()
{
	// Assign variables
	this->type = type;

	// Load mesh and texture for Exit model
	IAnimatedMesh* mesh = smgr->getMesh("models/pill.obj");
	ITexture* texture = driver->getTexture("textures/lethargy_pill.png");

	// Create model for player
	shared_ptr<Model> model = make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);
}

void Pill::update()
{
}

void Pill::draw()
{
	// Exit modl
	shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->rotation += vector3df(0.125, 0.25, 0.5) * deltaTime;
}