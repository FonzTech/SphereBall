#include "EngineObject.h"
#include "Utility.h"

IrrlichtDevice* EngineObject::device = nullptr;
IVideoDriver* EngineObject::driver = nullptr;
ISceneManager* EngineObject::smgr = nullptr;
IGUIEnvironment* EngineObject::guienv = nullptr;

void EngineObject::playSound(const std::string& key, const vector3df* position)
{
	if (position == nullptr)
	{
		sounds[key]->setRelativeToListener(true);
		sounds[key]->setPosition(sf::Vector3f(0, 0, 0));
	}
	else
	{
		sounds[key]->setRelativeToListener(false);
		sounds[key]->setPosition(utility::irrVectorToSf(*position));
	}
	sounds[key]->play();
}

void EngineObject::setEngineInstances(IrrlichtDevice* device, ISceneManager* smgr, IGUIEnvironment* guienv)
{
	EngineObject::device = device;
	EngineObject::driver = device->getVideoDriver();
	EngineObject::smgr = smgr;
	EngineObject::guienv = guienv;
}