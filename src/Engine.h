#ifndef ENGINE_H
#define ENGINE_H

#include <irrlicht.h>
#include <vector>
#include "EventManager.h"

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