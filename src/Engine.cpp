#include "Engine.h"
#include "ScreenQuadSceneNode.h"
#include "Utility.h"
#include "RoomManager.h"
#include "SoundManager.h"
#include "SharedData.h"
#include "Camera.h"

// Default values
const wchar_t* Engine::WINDOW_TITLE = L"SphereBall - Demo";
std::shared_ptr<Engine> Engine::singleton = nullptr;

Engine::Engine()
{
	// Initialize engine subsystems
	EventManager::singleton = std::make_shared<EventManager>();
	RoomManager::singleton = std::make_shared<RoomManager>();
	SoundManager::singleton = std::make_shared<SoundManager>();
	SharedData::singleton = std::make_shared<SharedData>();
	Camera::singleton = std::make_shared<Camera>();

	// Initialize variables and pointers
	sceneRtt = nullptr;
}

void Engine::createPostProcessingMaterial()
{
	postProcessing = std::make_unique<PostProcessing>(this);

	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	postProcessingMaterial = gpu->addHighLevelShaderMaterialFromFiles("shaders/scene.vs", "shaders/scene.fs", postProcessing.get());
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

bool Engine::setupComponents()
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

	// Setup material for post-processing
	createPostProcessingMaterial();

	// Bind post processing functions
	{
		std::function<void(const json&)> callback = [this](const json& data)
		{
			f32 params[2];

			data.at("speed").get_to(params[0]);
			data.at("strength").get_to(params[1]);

			postProcessing->waveSpeed = params[0];
			postProcessing->waveStrength = params[1];
		};
		SharedData::singleton->setPostProcessingCallback(KEY_PP_WAVE, callback);
	}

	// Check for render to target support
	return driver->queryFeature(video::EVDF_RENDER_TO_TARGET);
}

void Engine::loop()
{
	// Setup camera
	Camera::singleton->position = vector3df(0, 40, -100);
	Camera::singleton->lookAt = vector3df(0, 40, -100);

	// Loop while game is still running
	while (device->run() && RoomManager::singleton->isProgramRunning)
	{
		// Add new render target for window
		{
			if (sceneRtt != nullptr)
			{
				driver->removeTexture(sceneRtt);
				sceneRtt = nullptr;
			}

			// Get window size
			dimension2du windowSize = utility::getWindowSize<u32>(driver);

			// Create new render target
			sceneRtt = driver->addRenderTargetTexture(windowSize);

			// Make RT curren
			driver->setRenderTarget(sceneRtt);
		}

		// Now delta time
		u32 now = device->getTimer()->getTime();
		deltaTime = now - deltaTime;

		// Update post processing manager
		postProcessing->update((f32)deltaTime);

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
			std::shared_ptr<GameObject> &gameObject = RoomManager::singleton->gameObjects[i];

			// Affect drawing of game object
			gameObject->draw();

			// Add all game object's models to the scene
			for (std::shared_ptr<Model> &model : gameObject->models)
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

		// Add camera scene node
		smgr->addCameraSceneNode(0, Camera::singleton->position, Camera::singleton->lookAt);

		// Draw the entire scene
		smgr->drawAll();

		// Draw the entire GUI environment produced by game objects
		guienv->drawAll();

		// Work on scene render target
		{
			// Clear the entire scene
			smgr->clear();

			// Clear all the GUI environment produced by game objects
			guienv->clear();

			// Set default render target
			driver->setRenderTarget(0);

			// Display game surface
			ScreenQuadSceneNode screenQuad(smgr->getRootSceneNode(), smgr, -1);
			screenQuad.ChangeMaterialType((video::E_MATERIAL_TYPE) postProcessingMaterial);
			screenQuad.getMaterial(0).setTexture(0, sceneRtt);

			// Draw scene RTT quad
			screenQuad.render();
		}

		// Set Delta Time
		SharedData::singleton->update((f32)deltaTime);

		// Build required GUI from SharedData
		SharedData::singleton->buildGUI();

		// Draw the entire GUI environment produced by engine objects which are NOT game objects
		guienv->drawAll();

		// End scene after all
		driver->endScene();

		// Clear scene manager
		smgr->clear();

		// Clear GUI environment
		guienv->clear();

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

Engine::PostProcessing::PostProcessing(Engine* engine)
{
	// Set engine reference
	this->engine = engine;

	// Initialize variables
	waveTime = 0.0f;
	waveSpeed = 0.0f;
	waveStrength = 0.0f;
}

void Engine::PostProcessing::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{
	// Set shader values
	s32 layer0 = 0;
	services->setPixelShaderConstant("tex", (s32*)&layer0, 1);
	services->setPixelShaderConstant("time", &waveTime, 1);
	services->setPixelShaderConstant("strength", &waveStrength, 1);
}

void Engine::PostProcessing::update(f32 deltaTime)
{
	// Increase time for wave effect
	waveTime += deltaTime;

	// Decrease wave strength effect
	waveStrength -= waveSpeed * deltaTime;
	if (waveStrength < 0.0f)
	{
		waveStrength = 0.0f;
	}
}