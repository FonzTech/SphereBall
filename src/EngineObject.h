#ifndef ENGINEOBJECT_H
#define ENGINEOBJECT_H

#include <memory>
#include <unordered_map>
#include <string>

#include <irrlicht.h>
#include <SFML/Audio.hpp>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

class EngineObject
{
protected:

	// Map to hold sounds
	std::unordered_map<std::string, std::shared_ptr<sf::Sound>> sounds;

	// Play spatial sound
	std::shared_ptr<sf::Sound> playSound(const std::string& key, const vector3df* position = nullptr, const bool isMusic = false);

public:

	// Persistent object from game engine
	static IrrlichtDevice* device;
	static IVideoDriver* driver;
	static ISceneManager* smgr;
	static IGUIEnvironment* guienv;

	// Set engine instances
	static void setEngineInstances(IrrlichtDevice* device, ISceneManager* smgr, IGUIEnvironment* guienv);

	// Delta time
	f32 deltaTime;
};

#endif // ENGINEOBJECT_H