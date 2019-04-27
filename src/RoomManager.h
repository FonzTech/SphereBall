#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include "GameObject.h"

class RoomManager
{
protected:

	// Array to hold pickupable item keys
	std::vector<std::string> pickupableItemKeys;

	// Map structure to hold factory pattern for game objects
	std::unordered_map<std::string, std::function<std::shared_ptr<GameObject>(const json& jsonData)>> gameObjectFactory;

	// Room name holder
	std::string roomName;

public:

	// Main Menu static room name value
	static std::string ROOM_MAIN_MENU;

	// Singleton holder
	static std::shared_ptr<RoomManager> singleton;

	// Constructor
	RoomManager();

	// Variable to check if program is actually running
	bool isProgramRunning;

	// Vector to hold all active game objects
	std::vector<std::shared_ptr<GameObject>> gameObjects;

	// Method to load room
	void loadRoom(const std::string roomToLoad);

	// Method to restart room
	void restartRoom();
};

#endif // ROOMMANAGER_H