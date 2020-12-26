#ifdef _IRR_ANDROID_PLATFORM_

#include <android_native_app_glue.h>
#include "android_tools.h"
#include "android/window.h"

#endif

#include "Engine.h"
#include "RoomManager.h"

int game_main(void* privateData)
{
	// Initialize engine components
	Engine::singleton = std::make_shared<Engine>();

	// Start device with best target system renderer
	if (!Engine::singleton->startDevice(privateData))
	{
		printf("Cannot initialize graphic device\n");
		return EXIT_FAILURE;
	}

	// Make window appear
	if (!Engine::singleton->setupComponents())
	{
		printf("Your system does not support required components\n");
		return EXIT_FAILURE;
	}

	// Load first room
	RoomManager::singleton->loadRoom(RoomManager::ROOM_MAIN_MENU);

	// Main game loop
	Engine::singleton->loop();

	/*
		// Add game objects
		shared_ptr<Player> player(new Player(engine->getVideoDriver(), engine->getSceneManager()));
		player->position = vector3df(40, 30, 0);
		GameObject::gameObjects.push_back(player);

		shared_ptr<Solid> solid(new Solid(engine->getVideoDriver(), engine->getSceneManager()));
		solid->position = vector3df(64, solid->getModelAt(0)->mesh->getBoundingBox().getExtent().Y * 2.5f, 0);
		GameObject::gameObjects.push_back(solid);

		for (unsigned int i = 2; i < 6; ++i)
		{
			shared_ptr<Solid> solid(new Solid(engine->getVideoDriver(), engine->getSceneManager()));
			solid->position += vector3df(i * solid->getModelAt(0)->mesh->getBoundingBox().getExtent().X, 0, 0);
			GameObject::gameObjects.push_back(solid);
		}

		for (unsigned int i = 2; i < 3; ++i)
		{
			shared_ptr<Solid> solid(new Solid(engine->getVideoDriver(), engine->getSceneManager()));
			aabbox3df box = solid->getModelAt(0)->mesh->getBoundingBox();

			solid->position += vector3df(-box.getExtent().X, i * box.getExtent().Y, 0);
			GameObject::gameObjects.push_back(solid);
		}

		for (unsigned int i = 1; i < 3; ++i)
		{
			shared_ptr<Solid> solid(new Solid(engine->getVideoDriver(), engine->getSceneManager()));
			aabbox3df box = solid->getModelAt(0)->mesh->getBoundingBox();

			solid->position += vector3df(box.getExtent().X * 6, i * box.getExtent().Y, 0);
			GameObject::gameObjects.push_back(solid);
		}
	*/

	return EXIT_SUCCESS;
}

#ifdef _IRR_ANDROID_PLATFORM_

void android_main(android_app* app)
{
    // Make sure glue isn't stripped.
    app_dummy();

    // Game main function
    game_main(app);
}

#endif

#ifdef _WIN32

int main(int argc, char** argv)
{
	game_main(nullptr);
}

#endif