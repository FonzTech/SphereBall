#include "Engine.h"
#include "ScreenQuadSceneNode.h"
#include "Utility.h"
#include "RoomManager.h"
#include "SoundManager.h"
#include "SharedData.h"
#include "Camera.h"
#include "Editor.h"

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

bool Engine::startDevice(void* privateData)
{
	// Create device and assign
	SIrrlichtCreationParameters params;
	params.WindowSize = dimension2d<u32>(1920, 1080);
	params.Bits = 32;
	params.Vsync = true;
	params.EventReceiver = EventManager::singleton.get();

#ifdef _IRR_ANDROID_PLATFORM_
	if (privateData != nullptr)
	{
		params.privateData = privateData;
	}
	params.DriverType = EDT_OGLES2;
#else
	params.DriverType = EDT_OPENGL;
#endif

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

		// Blur
		callback = [this](const json& data)
		{
			f32 params[2];

			data.at("mode").get_to(params[0]);
			data.at("factor").get_to(params[1]);

			postProcessing->blurMode = params[0];
			postProcessing->blurFactor = params[1];
		};
		SharedData::singleton->setPostProcessingCallback(KEY_PP_BLUR, callback);
	}

	// Check for render to target support
	return driver->queryFeature(EVDF_RENDER_TO_TARGET);
}

void Engine::loop()
{
	// Debug data
	bool setBBoxVisible = false;

	// Setup camera
	Camera::singleton->setPosition(vector3df(0, 40, -100));
	Camera::singleton->setLookAt(vector3df(0));

	// Loop while game is still running
	while (device->run() && RoomManager::singleton->isProgramRunning)
	{
		// Set debug data visible
		#if NDEBUG || _DEBUG
		if (EventManager::singleton->keyStates[KEY_KEY_P] == KEY_PRESSED)
		{
			setBBoxVisible = !setBBoxVisible;
		}
		// Restart room on key press (only for debug builds)
		if (EventManager::singleton->keyStates[KEY_KEY_R] == KEY_PRESSED)
		{
			RoomManager::singleton->restartRoom();
		}
		#endif

		// Get window size
		dimension2du windowSize;
		{
			s32 videoMode = Utility::getVideoMode(device);
			windowSize = videoMode == -1 ? dimension2du(Utility::getWindowSize<u32>(driver)) : device->getVideoModeList()->getVideoModeResolution(videoMode);
		}

		// Setup GUI RTT
		{
			if (SharedData::singleton->guiRtt != nullptr)
			{
				driver->removeTexture(SharedData::singleton->guiRtt);
			}

			SharedData::singleton->guiRtt = driver->addRenderTargetTexture(windowSize, "guiRtt", ECF_A8R8G8B8);
		}

		// Setup and MRT
		irr::core::array<IRenderTarget>* sceneRtts = &SharedData::singleton->sceneRtts;

		{
			// Remove all textures and clear array
			for (s32 i = sceneRtts->size() - 1; i >= 0; --i)
			{
				driver->removeTexture((*sceneRtts)[i].RenderTexture);
			}
			sceneRtts->clear();

			// Create new textures for render targets
			sceneRtts->push_back(driver->addRenderTargetTexture(windowSize, "colorRtt", ECF_A8R8G8B8));
			sceneRtts->push_back(driver->addRenderTargetTexture(windowSize, "ppRtt", ECF_A8R8G8B8));
		}

		// Make G-Buffer current
		driver->setRenderTarget(*sceneRtts);

		// Now delta time
		u32 now = device->getTimer()->getTime();
		deltaTime = now - deltaTime;

		// Update post processing manager
		postProcessing->update((f32)deltaTime);

		// Double buffered scene with clear color
		driver->beginScene(true, true, SColor(0, 0, 0, 0));

		// Add camera scene node
		smgr->addCameraSceneNode(0, Camera::singleton->getPosition(), Camera::singleton->getLookAt());

		/*
		// Search for level editor
		if (RoomManager::singleton->gameObjects.size() > 0)
		{
			// Check if level editor is NOT in the final index of the vector
			const auto& last = RoomManager::singleton->gameObjects.end() - 1;
			if (Editor::singleton != nullptr && *(last) != Editor::singleton)
			{
				// Swap level editor instance against the last element
				const auto& it = std::find(RoomManager::singleton->gameObjects.begin(), RoomManager::singleton->gameObjects.end(), Editor::singleton);
				std::iter_swap(it, last);
			}
		}
		*/

		// Cycle through all available game objects
		for (u32 i = 0; i != RoomManager::singleton->gameObjects.size(); ++i)
		{
			// Get game object
			std::shared_ptr<GameObject> go = RoomManager::singleton->gameObjects[i];

			// Set delta time for the current object
			go->deltaTime = (f32) deltaTime;

			// Update current game object
			if (!SharedData::singleton->isAppPaused())
			{
				go->update();
			}

			// Check if game object has been destroyed
			if (go->destroy)
			{
				RoomManager::singleton->gameObjects.erase(RoomManager::singleton->gameObjects.begin() + i);
				--i;
				continue;
			}

			// Affect drawing of game object
			go->draw();
			
			// Game Object is a SkyBox
			if (go->gameObjectIndex == KEY_GOI_SKYBOX)
			{
				auto& model = go->models.at(0);
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
				for (std::shared_ptr<Model> &model : go->models)
				{
					// Create scene node from this mesh
					IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(model->mesh, nullptr, -1, model->position, model->rotation, model->scale);

					// Add all texture layer for this mesh (obtained from model)
					if (node != nullptr)
					{
						// Debug informations
						if (setBBoxVisible)
						{
							node->setDebugDataVisible(irr::scene::EDS_BBOX);
						}

						// Set current frame position
						node->setCurrentFrame(model->currentFrame);

						// Apply texture to all layers
						for (auto &entry : model->textures)
						{
							node->setMaterialTexture(entry.first, entry.second);
							node->setMaterialFlag(EMF_LIGHTING, false);
							node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
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

			// Post update
			go->postUpdate();
		}

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
			screenQuad.getMaterial(0).setTexture(0, SharedData::singleton->guiRtt);
			screenQuad.getMaterial(0).setTexture(1, (*sceneRtts)[0].RenderTexture);
			screenQuad.getMaterial(0).setTexture(2, (*sceneRtts)[1].RenderTexture);

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
	blurMode = 0.0f;
	blurFactor = 0.0f;
}

void Engine::PostProcessing::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Set shader values
	s32 layers[] = { 0, 1, 2 };
	services->setPixelShaderConstant("guiRtt", &layers[0], 1);
	services->setPixelShaderConstant("colorRtt", &layers[1], 1);
	services->setPixelShaderConstant("ppRtt", &layers[2], 1);

	dimension2du rttSize = SharedData::singleton->sceneRtts[0].RenderTexture->getOriginalSize();
	vector2df resolution((f32)rttSize.Width, (f32)rttSize.Height);
	services->setPixelShaderConstant("resolution", &resolution.X, 2);

	services->setPixelShaderConstant("time", &ppTime, 1);
	services->setPixelShaderConstant("waveStrength", &waveStrength, 1);
	services->setPixelShaderConstant("ripplePoint", &ripplePoint.X, 3);

	services->setPixelShaderConstant("blurFactor", &blurFactor, 1);
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
	ripplePoint.Z -= 0.001f * deltaTime;
	if (ripplePoint.Z < 0.0f)
	{
		ripplePoint.Z = 0.0f;
	}

	// Manage blur effect
	if (blurMode)
	{
		blurFactor += deltaTime * 0.003f;
		if (blurFactor > 1.0f)
		{
			blurFactor = 1.0f;
		}
	}
	else
	{
		blurFactor -= deltaTime * 0.003f;
		if (blurFactor < 0.0f)
		{
			blurFactor = 0.0f;
		}
	}
}