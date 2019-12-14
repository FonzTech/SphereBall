#include <algorithm>

#include "Exit.h"
#include "SharedData.h"

std::shared_ptr<Exit> Exit::createInstance(const json &jsonData)
{
	return std::make_shared<Exit>();
}

Exit::Exit() : GameObject()
{
	// Initialize variables
	angle = 0;
	picked = 0;
	color = SColorf(1.0f, 0.0f, 0.0f);

	// Create custom material
	SpecializedShaderCallback* ssc = new SpecializedShaderCallback(this);

	IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	customMaterial = gpu->addHighLevelShaderMaterialFromFiles("shaders/standard.vs", "shaders/exit.fs", ssc, EMT_TRANSPARENT_ALPHA_CHANNEL);

	ssc->drop();

	// Load mesh and texture for Exit model
	IAnimatedMesh* mesh = smgr->getMesh("models/exit.obj");
	ITexture* texture = driver->getTexture("textures/exit.png");
	
	// Create model for exit
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = customMaterial;
	models.push_back(model);

	// Load mesh and texture for Base model
	mesh = smgr->getMesh("models/plane.obj");
	texture = driver->getTexture("textures/exit_base_red.png");

	// Create model for base
	model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->rotation = vector3df(90, 0, 0);
	model->scale = vector3df(1, 1, 1);
	model->material = getCommonBasicMaterial(EMT_SOLID);
	models.push_back(model);
}

void Exit::update()
{
	// Check for total amount of keys picked
	if (SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_PICKED) >= SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_TOTAL))
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

	// Fade away
	if (color.a < 1.0f)
	{
		color.a -= 0.005f * deltaTime;
	}
}

void Exit::draw()
{
	// Exit model
	std::shared_ptr<Model> model = models.at(0);
	model->position = color.a <= 0.0f ? vector3df(std::numeric_limits<f32>::infinity()) : position;
	model->rotation = vector3df(0, angle, 0);
	model->material = customMaterial;

	// Base model
	model = models.at(1);
	model->position = position + vector3df(0, -9.7f, 0);
}

void Exit::pick()
{
	// Mark item as picked
	picked = 1;

	// Replace texture
	models.at(1)->textures[0] = driver->getTexture("textures/exit_base_green.png");

	// Make exit color green
	color = SColorf(0.0f, 1.0f, 0.0f);
}

void Exit::fade()
{
	color.a = 0.999f;
}

Exit::SpecializedShaderCallback::SpecializedShaderCallback(Exit* exit)
{
	this->exit = exit;
}

void Exit::SpecializedShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Set texture layer
	s32 layer0 = 0;
	services->setPixelShaderConstant("tex", (s32*)&layer0, 1);

	// Set position
	services->setPixelShaderConstant("color", &exit->color.r, 4);
}