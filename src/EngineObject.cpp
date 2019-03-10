#include "EngineObject.h"
#include "Utility.h"

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

void EngineObject::setEngineInstances(ISceneManager* smgr, IGUIEnvironment* guienv)
{
	EngineObject::smgr = smgr;
	EngineObject::guienv = guienv;
	EngineObject::driver = smgr->getVideoDriver();
}