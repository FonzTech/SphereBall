#include "Pickup.h"
#include "SharedData.h"

Pickup::Pickup() : GameObject()
{
	// Initialize variables
	angle = 0;
	notPicked = true;
	soundIndex = "";

	// Load mesh and texture
	IAnimatedMesh* mesh = smgr->getMesh("models/plane.obj");
	ITexture* texture = driver->getTexture("textures/coin_glare.png");

	// Load plane model
	planeModel = std::make_shared<Model>(mesh);
	planeModel->addTexture(0, texture);
	planeModel->material = COMMON_BASIC_MATERIAL_VERTEX_ALPHA;
	planeModel->scale = vector3df(2, 2, 0);
}

void Pickup::update()
{
	// Update angle
	if (notPicked)
	{
		angle += 0.25f * deltaTime;
	}
}

void Pickup::draw()
{
	// Draw glow
	if (!notPicked)
	{
		std::shared_ptr<Model> &model = models.at(0);
		model->position = position;
		model->rotation = vector3df(0, angle, 0);

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

bool Pickup::pick()
{
	// Check if item has not been already picked
	if (notPicked)
	{
		// Mark item as unpickable
		notPicked = false;
		angle = 0;

		// Replace model with plane
		models.erase(models.begin());
		models.push_back(planeModel);

		return false;
	}
	
	// Item is already picked
	return true;
}

Pickup::SpecializedShaderCallback::SpecializedShaderCallback(Pickup* pickup)
{
	this->pickup = pickup;
}

void Pickup::SpecializedShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Set shader values
	if (pickup->notPicked)
	{
		f32 shaderValues[1] = { pickup->angle };
		s32 layer0 = 0;
		services->setPixelShaderConstant("tex", (s32*)&layer0, 1);
		services->setPixelShaderConstant("angle", shaderValues, 1);
	}
}