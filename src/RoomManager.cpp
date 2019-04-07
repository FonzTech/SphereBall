#include <fstream>
#include "RoomManager.h"
#include "SharedData.h"

#include "MainMenu.h"
#include "Player.h"
#include "Solid.h"
#include "SkyBox.h"
#include "Coin.h"
#include "Spikes.h"
#include "Exit.h"
#include "Key.h"
#include "Pill.h"
#include "Hourglass.h"

std::shared_ptr<RoomManager> RoomManager::singleton = nullptr;

std::string RoomManager::ROOM_MAIN_MENU = "main_menu";

RoomManager::RoomManager()
{
	// Create vector to hold game objects
	gameObjects = std::vector<std::shared_ptr<GameObject>>();

	// Populate map for game objects factory pattern
	gameObjectFactory["MainMenu"] = &MainMenu::createInstance;
	gameObjectFactory["Player"] = &Player::createInstance;
	gameObjectFactory["Solid"] = &Solid::createInstance;
	gameObjectFactory["SkyBox"] = &SkyBox::createInstance;
	gameObjectFactory["Coin"] = &Coin::createInstance;
	gameObjectFactory["Spikes"] = &Spikes::createInstance;
	gameObjectFactory["Exit"] = &Exit::createInstance;
	gameObjectFactory["Key"] = &Key::createInstance;
	gameObjectFactory["Pill"] = &Pill::createInstance;
	gameObjectFactory["Hourglass"] = &Hourglass::createInstance;

	// Initialize variables
	isProgramRunning = true;
}

void RoomManager::loadRoom(const std::string roomToLoad)
{
	// Read room from file and parse JSON
	json jsonData;
	{
		try
		{
			std::ifstream input("rooms/" + roomToLoad + ".json");
			input >> jsonData;
		}
		catch (json::exception e)
		{
			printf("Room %s could NOT be loaded.\n", roomToLoad.c_str());
			return;
		}
	}

	// Clear currently loaded room
	gameObjects.clear();

	// Clear game score values
	SharedData::singleton->clearGameScore();

	// Iterate through all available objects
	for (u32 i = 0; i < jsonData.size(); ++i)
	{
		// Get JSON Object at i-index
		json object = jsonData.at(i);

		// Check for object name
		std::string name;
		object.at("name").get_to(name);

		// SharedData configuration
		if (name == SharedData::ROOM_OBJECT_KEY)
		{
			// Parse "enable" array
			json items = object.at("enable");

			// Cycle through game objects
			for (json::iterator it = items.begin(); it != items.end(); ++it)
			{
				json item = (*it);
				SharedData::singleton->initGameScoreValue(item.at("key"), item.at("value"));
			}
		}
		// Regulary game object
		else
		{
			// std::unordered_map<std::string, std::function<shared_ptr<GameObject>(const json& jsonData)>>::const_iterator
			auto goIterator = gameObjectFactory.find(name);

			// Check if class has been found
			if (goIterator == gameObjectFactory.end())
			{
				printf("Class for '%s' was not found.\n", name.c_str());
			}
			else
			{
				// Get method to instantiate class
				std::function<std::shared_ptr<GameObject>(const json& jsonData)> classFunction = goIterator->second;

				// Get pointer to class instance
				std::shared_ptr<GameObject> instance = classFunction(object);

				// Assign common data
				instance->assignGameObjectCommonData(object.at("required"));

				// Insert into current room
				gameObjects.push_back(instance);
			}
		}
	}

	// Store current loaded room
	roomName = roomToLoad;
}

void RoomManager::restartRoom()
{
	loadRoom(roomName);
}