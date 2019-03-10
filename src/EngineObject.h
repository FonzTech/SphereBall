#ifndef ENGINEOBJECT_H
#define ENGINEOBJECT_H

#include <memory>
#include <unordered_map>
#include <string>

#include <irrlicht.h>
#include <SFML/Audio.hpp>

using namespace std;

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

class EngineObject
{
protected:

	// Map to hold sounds
	unordered_map<std::string, shared_ptr<sf::Sound>> sounds;

	// Play spatial sound
	void playSound(const std::string& key, const vector3df* position = nullptr);

public:

	// Persistent object from game engine
	static IVideoDriver* driver;
	static ISceneManager* smgr;
	static IGUIEnvironment* guienv;

	// Set engine instances
	static void setEngineInstances(ISceneManager* smgr, IGUIEnvironment* guienv);
};

#endif // ENGINEOBJECT_H