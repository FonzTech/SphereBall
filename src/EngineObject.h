#ifndef ENGINEOBJECT_H
#define ENGINEOBJECT_H

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

class EngineObject
{
public:

	// Persistent object from game engine
	static IVideoDriver* driver;
	static ISceneManager* smgr;
	static IGUIEnvironment* guienv;

	// Set engine instances
	static void setEngineInstances(ISceneManager* smgr, IGUIEnvironment* guienv);
};

#endif // ENGINEOBJECT_H