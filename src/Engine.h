#ifndef ENGINE_H
#define ENGINE_H

#include <irrlicht.h>
#include <nlohmann/json.hpp>
#include <vector>

#include "EventManager.h"
#include "ShaderCallback.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;

using nlohmann::json;

class Engine
{
protected:

	// ShaderCallBack
	class PostProcessing : public ShaderCallback
	{
	protected:
		Engine* engine;

		f32 ppTime;

	public:

		f32 waveSpeed;
		f32 waveStrength;
		vector3df ripplePoint;
		f32 blurMode;
		f32 blurFactor;

		PostProcessing(Engine* engine);

		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

		void update(f32 deltaTime);
	};

	// Public static class constants
	static const wchar_t* WINDOW_TITLE;

	// Delta time
	u32 deltaTime;

	// Post-Processing system
	std::unique_ptr<PostProcessing> postProcessing;
	s32 postProcessingMaterial;
	void createPostProcessingMaterial();

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
	bool startDevice(void* privateData);

	// Setup window and required managers
	bool setupComponents();

	// Loop system for game
	void loop();
};

#endif // ENGINE_H