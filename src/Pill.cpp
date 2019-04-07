#include "Pill.h"

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

Pill::Pill(u8 type) : GameObject()
{
	// Assign variables
	this->type = type;

	// Load mesh and texture for Exit model
	IAnimatedMesh* mesh = smgr->getMesh("models/pill.obj");
	ITexture* texture = driver->getTexture("textures/lethargy_pill.png");

	// Create sparkle shader
	SpecializedShaderCallback* ssc = new SpecializedShaderCallback(this);

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
}

void Pill::draw()
{
	// Exit model
	std::shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->rotation += vector3df(0.125f, 0.25f, 0.5f) * deltaTime;
}

Pill::SpecializedShaderCallback::SpecializedShaderCallback(Pill* pill)
{
	this->pill = pill;
}

void Pill::SpecializedShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Set texture layer
	s32 layer0 = 0;
	services->setPixelShaderConstant("tex", (s32*)&layer0, 1);

	// Set position
	services->setPixelShaderConstant("position", &pill->position.X, 3);
}