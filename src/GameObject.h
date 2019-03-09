#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <memory>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <SFML/Audio.hpp>

#include "EngineObject.h"
#include "Collision.h"
#include "Model.h"
#include "Utility.h"

using namespace std;

using nlohmann::json;

class GameObject : public EngineObject
{
protected:

	// Map to hold sounds
	std::unordered_map<std::string, shared_ptr<sf::Sound>> sounds;

	// Check for collision with another game object
	template <typename T>
	Collision checkBoundingBoxCollision(const vector<shared_ptr<GameObject>>& gameObjects, aabbox3df& rect, const std::function<bool(const GameObject* go)>& specializedCheck = nullptr)
	{
		// Create data structure to hold the return value
		Collision collision;

		// Get translated bounding box;
		utility::transformAABBox(rect, position);

		// Check for all of the other game objects
		for (const shared_ptr<GameObject> &gameObject : gameObjects)
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

	// Play spatial sound
	void playSound(const std::string& key, const vector3df* position = nullptr);

public:

	// Get instance of game object with parameters
	static shared_ptr<GameObject> createInstance(const json &jsonData);

	// Assign common room data for GameObject
	void assignGameObjectCommonData(const json& commonData);

	// Common data among GameObjects
	vector<shared_ptr<Model>> models;

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