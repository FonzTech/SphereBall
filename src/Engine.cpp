#include "Engine.h"
#include "Utility.h"
#include "RoomManager.h"
#include "SoundManager.h"
#include "SharedData.h"
#include "Camera.h"

// Default values
const wchar_t* Engine::WINDOW_TITLE = L"SphereBall - Demo";
shared_ptr<Engine> Engine::singleton = nullptr;

Engine::Engine()
{
	// Initialize engine subsystems
	EventManager::singleton = make_shared<EventManager>();
	RoomManager::singleton = make_shared<RoomManager>();
	SoundManager::singleton = make_shared<SoundManager>();
	SharedData::singleton = make_shared<SharedData>();
	Camera::singleton = make_shared<Camera>();
}

bool Engine::startDevice()
{
	// Create device and assign
	SIrrlichtCreationParameters params;
	params.DriverType = video::EDT_OPENGL;
	params.WindowSize = dimension2d<u32>(1920, 1080);
	params.Bits = 32;
	params.Vsync = true;
	params.EventReceiver = EventManager::singleton.get();

	device = createDeviceEx(params);
	device->setResizable(true);

	// Return validity check
	return device != nullptr;
}

void Engine::setupComponents()
{
	// Setup window caption
	device->setWindowCaption(WINDOW_TITLE);

	// Create and setup driver
	driver = device->getVideoDriver();
	driver->getMaterial2D().TextureLayer[0].BilinearFilter = true;
	driver->getMaterial2D().AntiAliasing = video::EAAM_FULL_BASIC;

	// Create required components
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	// Set engine instances
	EngineObject::setEngineInstances(device, smgr, guienv);

	// Load assets for SharedData
	SharedData::singleton->loadAssets();

	// Set initial delta time
	deltaTime = device->getTimer()->getTime();
}

void Engine::loop()
{
	// Setup camera
	Camera::singleton->position = vector3df(0, 40, -100);
	Camera::singleton->lookAt = vector3df(0, 40, -100);

	// Loop while game is still running
	while (device->run() && RoomManager::singleton->isProgramRunning)
	{
		// Now delta time
		u32 now = device->getTimer()->getTime();
		deltaTime = now - deltaTime;

		// Clear scene
		smgr->clear();

		// Clear GUI
		guienv->clear();

		// Double buffered scene with clear color
		driver->beginScene(true, true, SColor(255, 100, 101, 140));

		// Cycle through all available game objects
		for (size_t i = 0; i != RoomManager::singleton->gameObjects.size(); ++i)
		{
			// Set delta time for the current object
			RoomManager::singleton->gameObjects[i]->deltaTime = (f32) deltaTime;

			// Update current game object
			RoomManager::singleton->gameObjects[i]->update();

			// Check if game object has been destroyed
			if (RoomManager::singleton->gameObjects[i]->destroy)
			{
				RoomManager::singleton->gameObjects.erase(RoomManager::singleton->gameObjects.begin() + i);
				--i;
				continue;
			}

			// Get static reference to object
			shared_ptr<GameObject> &gameObject = RoomManager::singleton->gameObjects[i];

			// Affect drawing of game object
			gameObject->draw();

			// Add all game object's models to the scene
			for (shared_ptr<Model> &model : gameObject->models)
			{
				// Create scene node from this mesh
				IMeshSceneNode* node = smgr->addMeshSceneNode(model->mesh, nullptr, -1, model->position, model->rotation, model->scale);

				// Add all texture layer for this mesh (obtained from model)
				if (node != nullptr)
				{
					// Apply texture to all layers
					for (auto &entry : model->textures)
					{
						node->setMaterialTexture(entry.first, entry.second);
						node->setMaterialFlag(video::EMF_LIGHTING, false);
					}

					// Set material type, if availablew
					if (model->material != -1)
					{
						node->setMaterialType((video::E_MATERIAL_TYPE) model->material);
						node->setMaterialFlag(video::EMF_BLEND_OPERATION, true);
					}
				}
			}
		}

		// Set Delta Time
		SharedData::singleton->stepAnimations((f32) deltaTime);

		// Build required GUI from SharedData
		SharedData::singleton->buildGUI();

		// Add camera scene node
		smgr->addCameraSceneNode(0, Camera::singleton->position, Camera::singleton->lookAt);

		// Draw the entire scene
		smgr->drawAll();

		// Draw the entire GUI environment
		guienv->drawAll();

		// End scene after all
		driver->endScene();

		// Update key states
		EventManager::singleton->updateKeyStates();

		// Update delta time
		deltaTime = now;
	}

	// Clear subsystem pointers
	EventManager::singleton = nullptr;
	RoomManager::singleton = nullptr;
	SoundManager::singleton = nullptr;
	SharedData::singleton = nullptr;
	Camera::singleton = nullptr;

	// Destroy device object
	device->drop();

	// Clear engine pointer
	Engine::singleton = nullptr;
}