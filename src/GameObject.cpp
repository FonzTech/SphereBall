#include "GameObject.h"

shared_ptr<GameObject> GameObject::createInstance(const json &jsonData)
{
	return nullptr;
}

void GameObject::assignGameObjectCommonData(const json& commonData)
{
	for (u8 i = 0; i < 3; ++i)
	{
		std::string key = i == 0 ? "position" : i == 1 ? "rotation" : "scale";
		vector3df v(0.0f, 0.0f, 0.0f);

		if (commonData.find(key) == commonData.end())
			v = i == 2 ? vector3df(1, 1, 1) : vector3df(0, 0, 0);
		else
		{
			v.X = (float)commonData[key]["x"];
			v.Y = (float)commonData[key]["y"];
			v.Z = (float)commonData[key]["z"];
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
	models = vector<shared_ptr<Model>>();

	// Initialize variables
	destroy = false;
}

void GameObject::playSound(const std::string& key, const vector3df* position)
{
	if (position == nullptr)
	{
		sounds[key]->setRelativeToListener(true);
		sounds[key]->setPosition(sf::Vector3f(0, 0, 0));
	}
	else
	{
		sounds[key]->setRelativeToListener(false);
		sounds[key]->setPosition(utility::irrVectorToSf(*position));
	}
	sounds[key]->play();
}