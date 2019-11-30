#include "Teleporter.h"

const std::string Teleporter::VECTOR_COMPONENTS[] = { "x", "y", "z" };
const std::string Teleporter::COLOR_COMPONENTS[] = { "r", "g", "b" };

std::shared_ptr<Teleporter> Teleporter::createInstance(const json &jsonData)
{
	vector3df warp;
	SColorf color(0);

	try
	{
		// Get optional data
		const json & optional = jsonData.at("optional");

		// Get the warp coordinates
		const json & optWarp = optional.at("warp");
		{
			/*
				Get coordinate components from the "warp"
				JSON object.
			*/
			float c[3] = { 0 };
			for (u8 i = 0; i < 3; ++i)
			{
				optWarp.at(VECTOR_COMPONENTS[i]).get_to(c[i]);
			}
			warp = vector3df(c[0], c[1], c[2]);
		}

		// Get the color property
		const json & optColor = optional.at("color");
		{
			/*
				Get components without requiring to specify
				all the RGB values in the JSON level file.
			*/
			float c[3] = { 0 };
			for (u8 i = 0; i < 3; ++i)
			{
				try
				{
					optColor.at(COLOR_COMPONENTS[i]).get_to(c[i]);
				}
				catch (json::exception ex)
				{
				}
			}

			// Create color from above components
			color = SColorf(c[0], c[1], c[2]);
		}
	}
	catch (json::exception & e)
	{
		// printf("Teleporter - Exception while parsing \"optional\": %s\n", e.what());
	}
	return std::make_shared<Teleporter>(warp, color);
}

Teleporter::Teleporter(const vector3df & warp, const SColorf & color) : GameObject()
{
	// Create custom material from shader
	SpecializedShaderCallback* ssc = new SpecializedShaderCallback(this);

	IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	customMaterial = gpu->addHighLevelShaderMaterialFromFiles("shaders/standard.vs", "shaders/teleporter.fs", ssc);

	ssc->drop();

	// Load mesh and texture
	IAnimatedMesh* mesh = smgr->getMesh("models/teleporter.obj");
	ITexture* texture = driver->getTexture("textures/teleporter.png");

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = customMaterial;
	model->boundingBox = Utility::getMesh(smgr, "models/cube.x")->getBoundingBox();
	models.push_back(model);
	
	// Initialize variables
	this->warp = warp;
	this->color = color;
	angle = 0.0f;
}

void Teleporter::update()
{
	angle += 0.1570f * deltaTime;
}

void Teleporter::draw()
{
	std::shared_ptr<Model> &model = models.at(0);
	model->position = position;
	model->rotation = vector3df(0, angle, 0);
}

Teleporter::SpecializedShaderCallback::SpecializedShaderCallback(Teleporter* teleporter)
{
	this->teleporter = teleporter;
}

void Teleporter::SpecializedShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Set texture layer
	s32 layer0 = 0;
	services->setPixelShaderConstant("tex", (s32*)&layer0, 1);

	// Set color
	services->setPixelShaderConstant("color", &teleporter->color.r, 3);
}