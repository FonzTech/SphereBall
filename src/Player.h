#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"

using namespace std;

class Player : public GameObject
{
protected:
	s8 direction;
	bool moving, falling;
	unique_ptr<vector3df> fallLine;
	std::function<bool(const GameObject* go)> coinCollisionCheck;

public:
	// Constructor
	Player();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static shared_ptr<Player> createInstance(const json &jsonData);
};

#endif // PLAYER_H