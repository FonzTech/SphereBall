#include "Coin.h"
#include "SharedData.h"

std::shared_ptr<Coin> Coin::createInstance(const json &jsonData)
{
	return std::make_shared<Coin>();
}

Coin::Coin() : GameObject()
{
	// Initialize variables
	angle = 0;
	notPicked = true;

	// Load mesh and texture
	IAnimatedMesh* mesh = smgr->getMesh("models/coin.obj");
	ITexture* texture = driver->getTexture("textures/coin.png");

	// Create sparkle shader
	SpecializedShaderCallback* ssc = new SpecializedShaderCallback(this);

	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	customShader = gpu->addHighLevelShaderMaterialFromFiles("shaders/standard.vs", "shaders/coin.fs", ssc);

	ssc->drop();

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = customShader;
	models.push_back(model);

	// Load mesh and texture
	mesh = smgr->getMesh("models/plane.obj");
	texture = driver->getTexture("textures/coin_glare.png");

	// Load plane model
	planeModel = std::make_shared<Model>(mesh);
	planeModel->addTexture(0, texture);
	planeModel->material = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	planeModel->scale = vector3df(2, 2, 0);
}

void Coin::update()
{
	// Update angle
	if (notPicked)
	{
		angle += 0.25f * deltaTime;
	}
}

void Coin::draw()
{
	std::shared_ptr<Model> &model = models.at(0);
	model->position = position;
	model->rotation = vector3df(0, angle, 0);

	if (!notPicked)
	{
		f32 s = (angle > 0.5f ? -0.05f : 0.05f) * deltaTime;
		model->scale += vector3df(s, s, 0);

		if (model->scale.X >= 16)
		{
			angle = 1;
		}
		else if (model->scale.X <= 0)
		{
			destroy = true;
		}
	}
}

void Coin::pick()
{
	// Increment coin counter by one
	SharedData::singleton->updateGameScoreValue(KEY_GUI_COIN, 1);

	// Mark item as unpickable
	notPicked = false;
	angle = 0;

	// Replace model with plane
	models.erase(models.begin());
	models.push_back(planeModel);
}

Coin::SpecializedShaderCallback::SpecializedShaderCallback(Coin* coin)
{
	this->coin = coin;
}

void Coin::SpecializedShaderCallback::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{
	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Set shader values
	if (coin->notPicked)
	{
		f32 shaderValues[1] = { coin->angle };
		s32 layer0 = 0;
		services->setPixelShaderConstant("tex", (s32*) &layer0, 1);
		services->setPixelShaderConstant("angle", shaderValues, 1);
	}
}