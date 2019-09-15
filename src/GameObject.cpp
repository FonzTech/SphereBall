#include "GameObject.h"

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
	destroy = false;
}

aabbox3df GameObject::getBoundingBox()
{
	return models.at(0)->mesh->getBoundingBox();
}