#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include "GameObject.h"

using namespace std;

class RoomManager
{
protected:
	// Map structure to hold factory pattern for game objects
	std::unordered_map<std::string, std::function<shared_ptr<GameObject>(const json& jsonData)>> gameObjectFactory;

public:
	// Singleton holder
	static shared_ptr<RoomManager> singleton;

	// Constructor
	RoomManager();

	// Variable to check if program is actually running
	bool isProgramRunning;

	// Vector to hold all active game objects
	vector<shared_ptr<GameObject>> gameObjects;

	// Method to load room
	void loadRoom(std::string name);
};

#endif // ROOMMANAGER_H