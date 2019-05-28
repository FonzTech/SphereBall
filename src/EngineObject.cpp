#include "EngineObject.h"
#include "Utility.h"
#include "SoundManager.h"

IrrlichtDevice* EngineObject::device = nullptr;
IVideoDriver* EngineObject::driver = nullptr;
ISceneManager* EngineObject::smgr = nullptr;
IGUIEnvironment* EngineObject::guienv = nullptr;

std::shared_ptr<sf::Sound> EngineObject::playSound(const std::string& key, const vector3df* position, const bool isMusic)
{
	std::shared_ptr<sf::Sound> sound = sounds[key];

	if (position == nullptr)
	{
		sound->setRelativeToListener(true);
		sound->setPosition(sf::Vector3f(0, 0, 0));
	}
	else
	{
		sound->setRelativeToListener(false);
		sound->setPosition(utility::irrVectorToSf(*position));
	}

	sound->setVolume(SoundManager::singleton->volumeLevels[isMusic ? KEY_SETTING_MUSIC : KEY_SETTING_SOUND]);
	sound->play();

	return sound;
}

void EngineObject::setEngineInstances(IrrlichtDevice* device, ISceneManager* smgr, IGUIEnvironment* guienv)
{
	EngineObject::device = device;
	EngineObject::driver = device->getVideoDriver();
	EngineObject::smgr = smgr;
	EngineObject::guienv = guienv;
}