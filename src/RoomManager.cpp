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
#include "Fruit.h"
#include "Fire.h"
#include "Teleporter.h"
#include "Editor.h"

std::shared_ptr<RoomManager> RoomManager::singleton = nullptr;

const std::string RoomManager::LEVEL_PREFIX = "level_";

const std::string RoomManager::ROOM_MAIN_MENU = "main_menu";
const std::string RoomManager::ROOM_EDITOR = "editor";

RoomManager::RoomManager()
{
	// Fill array for pickupable item keysd
	pickupableItemKeys.push_back("Coin");
	pickupableItemKeys.push_back("Hourglass");
	pickupableItemKeys.push_back("Key");

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
	gameObjectFactory["Fruit"] = &Fruit::createInstance;
	gameObjectFactory["Fire"] = &Fire::createInstance;
	gameObjectFactory["Teleporter"] = &Teleporter::createInstance;
	gameObjectFactory["Editor"] = &Editor::createInstance;

	// Initialize variables
	isProgramRunning = true;
	levelIndex = 0;
}

u32 RoomManager::getCurrentLevelIndex()
{
	return levelIndex;
}

void RoomManager::loadRoom(const std::string roomToLoad)
{
	// Check if requested room is a level
	if (Utility::startsWith(roomToLoad, LEVEL_PREFIX))
	{
		const std::string suffix = roomToLoad.substr(LEVEL_PREFIX.length());
		levelIndex = std::atoi(suffix.c_str());
	}
	else
	{
		levelIndex = 0;
	}

	// Read room from file and parse JSON
	nlohmann::json jsonData;
	{
		try
		{
			std::ifstream input("rooms/" + roomToLoad + ".json");
			input >> jsonData;
		}
		catch (nlohmann::json::exception e)
		{
			printf("Room %s could NOT be loaded.\n", roomToLoad.c_str());
			return;
		}
	}

	// Clear currently loaded room
	gameObjects.clear();

	// Clear game score values
	SharedData::singleton->clearGameScore();

	// Reset room's lower bound
	lowerBound = 0.0f;

	// Iterate through all available objects
	for (u32 i = 0; i < jsonData.size(); ++i)
	{
		// Get JSON Object at i-index
		nlohmann::json object = jsonData.at(i);

		// Check for object name
		std::string name;
		object.at("name").get_to(name);

		// SharedData configuration
		if (name == SharedData::ROOM_OBJECT_KEY)
		{
			// Parse "enable" array
			nlohmann::json items = object.at("enable");

			// Cycle through game objects
			for (nlohmann::json::iterator it = items.begin(); it != items.end(); ++it)
			{
				nlohmann::json item = (*it);
				SharedData::singleton->initGameScoreValue(item.at("key"), item.at("value"));
			}

			// Initialize pickupable items counter
			SharedData::singleton->initGameScoreValue(KEY_SCORE_ITEMS_PICKED, 0);
			SharedData::singleton->initGameScoreValue(KEY_SCORE_ITEMS_MAX, 0);
			SharedData::singleton->initGameScoreValue(KEY_SCORE_POINTS, 0);
		}
		// Ordinary game object
		else
		{
			// std::unordered_map<std::string, std::function<shared_ptr<GameObject>(const nlohmann::json& jsonData)>>::const_iterator
			auto goIterator = gameObjectFactory.find(name);

			// Check if class has been found
			if (goIterator == gameObjectFactory.end())
			{
				printf("Class for '%s' was not found.\n", name.c_str());
			}
			else
			{
				// Get method to instantiate class
				std::function<std::shared_ptr<GameObject>(const nlohmann::json& jsonData)> classFunction = goIterator->second;

				// Get pointer to class instance
				std::shared_ptr<GameObject> instance = classFunction(object);

				// Check for editor game object
				if (goIterator->first == "Editor")
				{
					Editor::singleton = instance;
				}

				// Check if item is a pickup
				if (std::find(pickupableItemKeys.begin(), pickupableItemKeys.end(), goIterator->first) != pickupableItemKeys.end())
				{
					SharedData::singleton->updateGameScoreValue(KEY_SCORE_ITEMS_MAX, 1);
				}

				// Assign common data
				try
				{
					instance->assignGameObjectCommonData(object.at("required"));
				}
				catch (nlohmann::json::exception e)
				{
				}

				// Insert into current room
				gameObjects.push_back(instance);

				// Check for solid game object to minimize room's lower bound
				if (goIterator->first == "Solid")
				{
					lowerBound = std::min(lowerBound, instance->position.Y);
				}
			}
		}
	}

	// Move lower bound a bit lower
	lowerBound -= 40.0f;

	// Store current loaded room
	roomName = roomToLoad;
}

void RoomManager::restartRoom()
{
	loadRoom(roomName);
}

void RoomManager::jumpToNextLevel()
{
	loadRoom(LEVEL_PREFIX + std::to_string(levelIndex + 1));
}

bool RoomManager::isCurrentRoomALevel()
{
	return levelIndex > 0;
}