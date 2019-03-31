#ifndef ENGINE_H
#define ENGINE_H

#include <irrlicht.h>
#include <vector>

#include "EventManager.h"
#include "ShaderCallback.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;

class Engine
{
protected:
	// Public static class constants
	static const wchar_t* WINDOW_TITLE;

	// Delta time
	u32 deltaTime;

	// Global render target
	ITexture* sceneRtt;

	// Post-Processing material
	s32 postProcessingMaterial;

	// Create Post-Processing material
	void createPostProcessingMaterial();

	// ShaderCallBack
	class SpecializedShaderCallback : public ShaderCallback
	{
	protected:
		Engine* engine;

	public:
		SpecializedShaderCallback(Engine* engine);
		virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData);
	};

public:
	// Singleton pattern
	static std::shared_ptr<Engine> singleton;

	// Constructor
	Engine();

	// Engine objects
	IrrlichtDevice* device;
	IVideoDriver* driver;
	ISceneManager* smgr;
	IGUIEnvironment* guienv;

	// Start device with best target system target
	bool startDevice();

	// Setup window and required managers
	bool setupComponents();

	// Loop system for game
	void loop();
};

#endif // ENGINE_H