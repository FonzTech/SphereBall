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
}

void Engine::createPostProcessingMaterial()
{
	postProcessing = std::make_unique<PostProcessing>(this);

	IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	postProcessingMaterial = gpu->addHighLevelShaderMaterialFromFiles("shaders/scene.vs", "shaders/scene.fs", postProcessing.get());
}

bool Engine::startDevice()
{
	// Create device and assign
	SIrrlichtCreationParameters params;
	params.DriverType = EDT_OPENGL;
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
	driver->getMaterial2D().AntiAliasing = EAAM_FULL_BASIC;

	// Create required components
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	// Set engine instances
	EngineObject::setEngineInstances(device, smgr, guienv);
	GameObject::COMMON_EMT_MATERIAL_SOLID = GameObject::getCommonBasicMaterial();
	GameObject::COMMON_EMT_MATERIAL_VERTEX_ALPHA = GameObject::getCommonBasicMaterial(EMT_TRANSPARENT_VERTEX_ALPHA);
	GameObject::COMMON_EMT_TRANSPARENT_ADD_COLOR = GameObject::getCommonBasicMaterial(EMT_TRANSPARENT_ADD_COLOR);

	// Load assets for SharedData
	SharedData::singleton->loadAssets();

	// Set initial delta time
	deltaTime = device->getTimer()->getTime();

	// Setup material for post-processing
	createPostProcessingMaterial();

	// Bind post processing functions
	{
		// Declare function
		std::function<void(const json&)> callback;

		// Wave		
		callback = [this](const json& data)
		{
			f32 params[2];

			data.at("speed").get_to(params[0]);
			data.at("strength").get_to(params[1]);

			postProcessing->waveSpeed = params[0];
			postProcessing->waveStrength = params[1];
		};
		SharedData::singleton->setPostProcessingCallback(KEY_PP_WAVE, callback);

		// Ripple
		callback = [this](const json& data)
		{
			f32 params[3];

			data.at("x").get_to(params[0]);
			data.at("y").get_to(params[1]);
			data.at("z").get_to(params[2]);

			postProcessing->ripplePoint = vector3df(params[0], params[1], params[2]);
		};
		SharedData::singleton->setPostProcessingCallback(KEY_PP_RIPPLE, callback);
	}

	// Check for render to target support
	return driver->queryFeature(EVDF_RENDER_TO_TARGET);
}

void Engine::loop()
{
	// Setup camera
	Camera::singleton->position = vector3df(0, 40, -100);
	Camera::singleton->lookAt = vector3df(0);

	// Loop while game is still running
	while (device->run() && RoomManager::singleton->isProgramRunning)
	{
		// Get window size
		dimension2du windowSize;
		{
			s32 videoMode = Utility::getVideoMode(device);
			windowSize = videoMode == -1 ? dimension2du(Utility::getWindowSize<u32>(driver)) : device->getVideoModeList()->getVideoModeResolution(videoMode);
		}

		// Setup MRT
		{
			// Remove all textures and clear array
			for (s32 i = sceneRtts.size() - 1; i >= 0; --i)
			{
				driver->removeTexture(sceneRtts[i].RenderTexture);
			}
			sceneRtts.clear();

			// Create new textures for render targets
			sceneRtts.push_back(driver->addRenderTargetTexture(windowSize, "colorRtt", ECF_A8R8G8B8));
			sceneRtts.push_back(driver->addRenderTargetTexture(windowSize, "ppRtt", ECF_A8R8G8B8));
		}

		// Make G-Buffer current
		driver->setRenderTarget(sceneRtts);

		// Now delta time
		u32 now = device->getTimer()->getTime();
		deltaTime = now - deltaTime;

		// Update post processing manager
		postProcessing->update((f32)deltaTime);

		// Double buffered scene with clear color
		driver->beginScene(true, true, SColor(0, 0, 0, 0));

		// Cycle through all available game objects
		std::vector<std::shared_ptr<GameObject>>::iterator it = RoomManager::singleton->gameObjects.begin();
		while (it != RoomManager::singleton->gameObjects.end())
		{
			// Set delta time for the current object
			(*it)->deltaTime = (f32) deltaTime;

			// Update current game object
			(*it)->update();

			// Check if game object has been destroyed
			if ((*it)->destroy)
			{
				it = RoomManager::singleton->gameObjects.erase(it);
				continue;
			}

			// Affect drawing of game object
			(*it)->draw();
			
			// Game Object is a SkyBox
			if ((*it)->gameObjectIndex == KEY_GOI_SKYBOX)
			{
				auto& model = (*it)->models.at(0);
				auto& textures = model->textures;

				ISceneNode* node = smgr->addSkyBoxSceneNode(textures[0], textures[1], textures[2], textures[3], textures[4], textures[5]);
				node->setRotation(model->rotation);

				node->setMaterialType((E_MATERIAL_TYPE)model->material);
				node->setMaterialFlag(EMF_BLEND_OPERATION, true);
				node->setMaterialFlag(EMF_LIGHTING, false);
			}
			// Ordinary game object
			else
			{
				// Add all game object's models to the scene
				for (std::shared_ptr<Model> &model : (*it)->models)
				{
					// Create scene node from this mesh
					IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(model->mesh, nullptr, -1, model->position, model->rotation, model->scale);

					// Add all texture layer for this mesh (obtained from model)
					if (node != nullptr)
					{
						// Set current frame position
						node->setCurrentFrame(model->currentFrame);

						// Apply texture to all layers
						for (auto &entry : model->textures)
						{
							node->setMaterialTexture(entry.first, entry.second);
							node->setMaterialFlag(EMF_LIGHTING, false);
						}

						// Set material type, if available
						if (model->material != -1)
						{
							node->setMaterialType((E_MATERIAL_TYPE)model->material);
							node->setMaterialFlag(EMF_BLEND_OPERATION, true);
						}
					}
				}
			}

			// Advance iterator
			++it;
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
			screenQuad.ChangeMaterialType((E_MATERIAL_TYPE)postProcessingMaterial);
			screenQuad.getMaterial(0).setTexture(0, sceneRtts[0].RenderTexture);
			screenQuad.getMaterial(0).setTexture(1, sceneRtts[1].RenderTexture);

			// Draw scene RTT quad
			screenQuad.render();
			screenQuad.remove();
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

		// Dispose unneeded resource for SharedData controller
		SharedData::singleton->disposeResourcesAtFrameEnd();

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
	ppTime = 0.0f;
	waveSpeed = 0.0f;
	waveStrength = 0.0f;
	ripplePoint = vector3df(0.0f);
}

void Engine::PostProcessing::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Set shader values
	s32 layer0 = 0;
	services->setPixelShaderConstant("colorRtt", &layer0, 1);

	s32 layer1 = 1;
	services->setPixelShaderConstant("ppRtt", &layer1, 1);

	services->setPixelShaderConstant("time", &ppTime, 1);
	services->setPixelShaderConstant("waveStrength", &waveStrength, 1);
	services->setPixelShaderConstant("ripplePoint", &ripplePoint.X, 3);
}

void Engine::PostProcessing::update(f32 deltaTime)
{
	// Increase time for wave effect
	ppTime += deltaTime;

	// Decrease wave strength effect
	waveStrength -= waveSpeed * deltaTime;
	if (waveStrength < 0.0f)
	{
		waveStrength = 0.0f;
	}

	// Decrease ripple strength effect
	ripplePoint.Z -= 0.002f * deltaTime;
	if (ripplePoint.Z < 0.0f)
	{
		ripplePoint.Z = 0.0f;
	}
}