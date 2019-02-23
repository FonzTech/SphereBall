#include "EngineObject.h"

IVideoDriver* EngineObject::driver = nullptr;
ISceneManager* EngineObject::smgr = nullptr;
IGUIEnvironment* EngineObject::guienv = nullptr;

void EngineObject::setEngineInstances(ISceneManager* smgr, IGUIEnvironment* guienv)
{
	EngineObject::smgr = smgr;
	EngineObject::guienv = guienv;
	EngineObject::driver = smgr->getVideoDriver();
}