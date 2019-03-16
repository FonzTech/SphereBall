#include "Pill.h"

s32 Pill::customMaterial = -1;

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

	// Create sparkle shader
	if (customMaterial == -1)
	{
		SpecializedShaderCallback* ssc = new SpecializedShaderCallback(this);

		video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
		customMaterial = gpu->addHighLevelShaderMaterialFromFiles("shaders/pill.vs", "shaders/pill.fs", ssc);

		ssc->drop();
	}

	// Create model for player
	shared_ptr<Model> model = make_shared<Model>(mesh);
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
	shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->rotation += vector3df(0.125, 0.25, 0.5) * deltaTime;
}

Pill::SpecializedShaderCallback::SpecializedShaderCallback(Pill* pill)
{
	this->pill = pill;
}

void Pill::SpecializedShaderCallback::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{
	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Set shader values
	s32 layer0 = 0;
	services->setPixelShaderConstant("tex", (s32*)&layer0, 1);

	f32 values[3];
	pill->position.getAs3Values(values);
	services->setPixelShaderConstant("position", values, 3);
}