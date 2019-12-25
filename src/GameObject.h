#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#define KEY_GOI_SKYBOX 1

#include <vector>
#include <nlohmann/json.hpp>

#include "EngineObject.h"
#include "ShaderCallback.h"
#include "Collision.h"
#include "Model.h"
#include "Utility.h"

using nlohmann::json;

class GameObject : public EngineObject
{
protected:

	/**
		This structure indicates the normal map texture index in the "textures" map of the model
		and the light power to be applied for shading. This also describes a limitation of this
		application, because a normal map is tied to the mesh, not the game object which uses it.
		Anyway, every game objects should have their primary model, which can have a normal map,
		in the first index of the "models" vector. A value of 0 indicates no normal map enabled.
	*/
	struct
	{
		s32 textureIndex = 0;
		f32 lightPower = 1.5f;
	} normalMapping;

	// Check for collision with another game object
	template <typename T>
	Collision checkBoundingBoxCollision(const std::vector<std::shared_ptr<GameObject>>& gameObjects, aabbox3df& rect, const std::function<bool(GameObject* go)>& specializedCheck = nullptr)
	{
		// Create data structure to hold the return value
		Collision collision;

		// Get translated bounding box;
		Utility::transformAABBox(rect, position);

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

			// Check if object has any model attached
			else if (gameObject->models.size() == 0)
			{
				continue;
			}

			// Get bounding box of mesh
			aabbox3df otherBox(gameObject->getBoundingBox());
			Utility::transformAABBox(otherBox, gameObject->position);

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

	/*
		Create a basic common material to apply transformation matrix in vertex shader and
		basic texture mapping in fragment shader. "standard.vs" and "standard.fs" are used.

		@param basicMaterial the basic material to create the shader from.

		@return material index to be used on mesh nodes.
	*/
	const s32 getCommonBasicMaterial(E_MATERIAL_TYPE basicMaterial = EMT_SOLID);

	// Get instance of game object with parameters
	static std::shared_ptr<GameObject> createInstance(const json &jsonData);

	// Assign common room data for GameObject
	void assignGameObjectCommonData(const json& commonData);

	// Common data among GameObjects
	std::vector<std::shared_ptr<Model>> models;

	// Object properties
	u8 gameObjectIndex;
	bool destroy;
	vector3df position;
	vector3df speed;

	// Constructor
	GameObject();

	// Update this game object
	virtual void update() = 0;

	// Make commits on drawable resources
	virtual void draw() = 0;

	// Bounding box getter
	virtual aabbox3df getBoundingBox();

	/**
		This method applies the routine for normal mapping, used in shader service
		inside this GameObject's subclasses.

		@param services the "IMaterialRendererServices" instance passed as argument by "OnSetConstants".
		@param model the model where to get the texture from.
	*/
	void applyNormalMapping(IMaterialRendererServices* services, const std::shared_ptr<Model> model);

	// ShaderCallBack
	class BasicShaderCallback : public ShaderCallback
	{
	protected:
		GameObject* go;

	public:
		BasicShaderCallback(GameObject* go);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	};
};

#endif // GAMEOBJECT_H