#include "GameObject.h"
#include "Camera.h"

const s32 GameObject::getCommonBasicMaterial(E_MATERIAL_TYPE basicMaterial)
{
	// Create basic shader
	BasicShaderCallback* bsc = new BasicShaderCallback(this);

	IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	s32 material = gpu->addHighLevelShaderMaterialFromFiles("shaders/standard.vs", "shaders/standard.fs", bsc, basicMaterial);

	bsc->drop();

	return material;
}

std::shared_ptr<GameObject> GameObject::createInstance(const json &jsonData)
{
	return nullptr;
}

void GameObject::assignGameObjectCommonData(const json& commonData)
{
	for (u8 i = 0; i < 3; ++i)
	{
		std::string key = i == 0 ? "position" : i == 1 ? "rotation" : "scale";
		vector3df v;

		const auto& iterator = commonData.find(key);
		if (iterator == commonData.end())
		{
			v = i == 2 ? vector3df(1, 1, 1) : vector3df(0, 0, 0);
		}
		else
		{
			iterator->at("x").get_to(v.X);
			iterator->at("y").get_to(v.Y);
			iterator->at("z").get_to(v.Z);
		}

		if (i == 0)
		{
			this->position = v;
		}
		else if (this->models.size() > 0)
		{
			if (i == 1)
			{
				this->models.at(0)->rotation = v;
			}
			else
			{
				this->models.at(0)->scale = v;
			}
		}
	}
}

GameObject::GameObject()
{
	// Initialize vector for models
	models = std::vector<std::shared_ptr<Model>>();

	// Initialize variables
	gameObjectIndex = 0;
	destroy = false;
}

void GameObject::postUpdate()
{
}

aabbox3df GameObject::getBoundingBox()
{
	return models.at(0)->mesh->getBoundingBox();
}

void GameObject::applyNormalMapping(IMaterialRendererServices* services, const std::shared_ptr<Model> model)
{
	// Apply normal map if required
	const bool useNormalMap = model->normalMapping.textureIndex > 0;
	services->setVertexShaderConstant("useNormalMap", &useNormalMap, 1);
	services->setPixelShaderConstant("useNormalMap", &useNormalMap, 1);

	// Check for normal map existance
	if (useNormalMap)
	{
		const vector3df p = Camera::singleton->getPosition();
		services->setVertexShaderConstant("eyePos", &p.X, 3);

		const vector3df lightDir(0, -1, 1);
		services->setVertexShaderConstant("lightDir", &lightDir.X, 3);

		const vector3df eyeDir = Camera::singleton->getLookAt() - p;
		services->setVertexShaderConstant("eyeDir", &eyeDir.X, 3);

		services->setPixelShaderConstant("lightPower", &model->normalMapping.lightPower, 1);
		services->setPixelShaderConstant("normalMap", &model->normalMapping.textureIndex, 1);
	}
}

GameObject::BasicShaderCallback::BasicShaderCallback(GameObject* go)
{
	this->go = go;
}

void GameObject::BasicShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Apply normal mapping if required
	if (go->models.size())
	{
		go->applyNormalMapping(services, go->models.at(0));
	}
}