#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <nlohmann/json.hpp>

#include "EngineObject.h"
#include "Collision.h"
#include "Model.h"
#include "Utility.h"

using nlohmann::json;

class GameObject : public EngineObject
{
protected:

	// Check for collision with another game object
	template <typename T>
	Collision checkBoundingBoxCollision(const std::vector<std::shared_ptr<GameObject>>& gameObjects, aabbox3df& rect, const std::function<bool(const GameObject* go)>& specializedCheck = nullptr)
	{
		// Create data structure to hold the return value
		Collision collision;

		// Get translated bounding box;
		utility::transformAABBox(rect, position);

		// Check for all of the other game objects
		for (const std::shared_ptr<GameObject> &gameObject : gameObjects)
		{
			// Check for class maching
			if (dynamic_cast<T*>(gameObject.get()) == nullptr)
			{
				continue;
			}

			// Check if object has been destroyed
			else if (gameObject->destroy)
			{
				continue;
			}

			// Get bounding box of mesh
			aabbox3df otherBox(gameObject->models.at(0)->mesh->getBoundingBox());
			utility::transformAABBox(otherBox, gameObject->position);

			// Check for intersection
			if (rect.intersectsWithBox(otherBox))
			{
				// Perform specialized checking if supplied
				if (specializedCheck != nullptr && !specializedCheck(gameObject.get()))
					continue;

				// Return collision information
				collision.engineObject = gameObject;
				collision.mainBoundingBox = models.at(0)->mesh->getBoundingBox();
				collision.otherBoundingBox = otherBox;
				return collision;
			}
		}

		// Return built object
		return collision;
	}

public:

	// Get instance of game object with parameters
	static std::shared_ptr<GameObject> createInstance(const json &jsonData);

	// Assign common room data for GameObject
	void assignGameObjectCommonData(const json& commonData);

	// Common data among GameObjects
	std::vector<std::shared_ptr<Model>> models;

	// Object properties
	bool destroy;
	f32 deltaTime;
	vector3df position;
	vector3df speed;

	// Constructor
	GameObject();

	// Update this game object
	virtual void update() = 0;

	// Make commits on drawable resources
	virtual void draw() = 0;
};

#endif // GAMEOBJECT_H