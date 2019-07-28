#include "Pill.h"
#include "SharedData.h"
#include "SoundManager.h"

std::shared_ptr<Pill> Pill::createInstance(const json &jsonData)
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
	return std::make_shared<Pill>(type);
}

Pill::Pill() : Pill(0)
{
}

Pill::Pill(u8 type) : Pickup()
{
	// Assign variables
	this->type = type;

	// Load mesh and texture for Exit model
	IAnimatedMesh* mesh = smgr->getMesh("models/pill.obj");
	ITexture* texture = driver->getTexture("textures/lethargy_pill.png");

	// Create sparkle shader
	PillShaderCallback* ssc = new PillShaderCallback(this);

	IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	customMaterial = gpu->addHighLevelShaderMaterialFromFiles("shaders/pill.vs", "shaders/pill.fs", ssc);

	ssc->drop();

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = customMaterial;
	models.push_back(model);
}

void Pill::update()
{
	Pickup::update();
}

void Pill::draw()
{
	Pickup::draw();

	// Exit model
	std::shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->rotation += vector3df(0.125f, 0.25f, 0.5f) * deltaTime;
}

bool Pill::pick()
{
	if (Pickup::pick())
	{
		return true;
	}

	// Trigger wave effect
	json data;
	data["speed"] = 0.00001f;
	data["strength"] = 0.05f;
	SharedData::singleton->triggerPostProcessingCallback(KEY_PP_WAVE, data);

	// Trigger fade out
	SharedData::singleton->startFade(false, nullptr, 1.0f);

	// Set sound to play
	soundIndex = KEY_SOUND_LETHARGY_PILL;

	return false;
}

Pill::PillShaderCallback::PillShaderCallback(Pill* pill)
{
	this->pill = pill;
}

void Pill::PillShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Set texture layer
	s32 layer0 = 0;
	services->setPixelShaderConstant("tex", (s32*)&layer0, 1);

	// Set position
	services->setPixelShaderConstant("position", &pill->position.X, 3);
}