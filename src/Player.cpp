#include "Player.h"

#include "EventManager.h"
#include "RoomManager.h"
#include "SoundManager.h"
#include "Camera.h"

#include "Solid.h"
#include "Coin.h"
#include "Utility.h"

shared_ptr<Player> Player::createInstance(const json &jsonData)
{
	return make_shared<Player>();
}

Player::Player() : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh = smgr->getMesh("models/sphere.obj");

	// Load texture
	ITexture* texture = driver->getTexture("textures/player.png");

	// Create model for player
	shared_ptr<Model> model = make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);

	// Initialize variables
	direction = 1;
	moving = falling = false;
	fallLine = nullptr;

	// Load sounds
	sounds["bounce"] = SoundManager::singleton->getSound("bounce");
	sounds["coin"] = SoundManager::singleton->getSound("coin");

	// Create specialized functions
	coinCollisionCheck = [](const GameObject* go)
	{
		Coin* coin = (Coin*)go;
		return coin->notPicked;
	};
}

void Player::update()
{
	// Get main model bounding box
	aabbox3df bbox = models.at(0)->mesh->getBoundingBox();

	// Make horizontal movements
	if (EventManager::singleton->keyStates[KEY_LEFT] >= KEY_PRESSED)
	{
		speed += vector3df(-0.01f, 0, 0);
		moving = true;
	}
	else if (EventManager::singleton->keyStates[KEY_RIGHT] >= KEY_PRESSED)
	{
		speed += vector3df(0.01f, 0, 0);
		moving = true;
	}
	else
		moving = false;

	// Check for direction
	if (speed.X < 0)
		direction = -1;
	if (speed.X > 0)
		direction = 1;

	// Decrease speed when not moving
	if (!moving && !falling)
	{
		// Left
		if (direction < 0)
		{
			speed += vector3df(0.01f, 0, 0);
			if (speed.X > 0)
				speed.X = 0;
		}
		// Right
		else
		{
			speed += vector3df(-0.01f, 0, 0);
			if (speed.X < 0)
				speed.X = 0;
		}
	}

	// Limit horizontal speed
	if (speed.X < -0.1f)
		speed.X = -0.1f;
	else if (speed.X > 0.1f)
		speed.X = 0.1f;

	// Check for input
	if (!falling && EventManager::singleton->keyStates[KEY_UP] == KEY_PRESSED)
	{
		// Setup vertical speed
		speed += vector3df(0, 0.2f, 0);

		// Set fall line above player
		vector3df fl(position + vector3df(0, bbox.getExtent().Y, 0));
		fallLine = unique_ptr<vector3df>(new vector3df(fl));

		// Turn on falling
		falling = true;
	}

	// Gravity
	if (falling)
	{
		if (fallLine != nullptr)
		{
			if (position.Y >= fallLine->Y)
				fallLine = nullptr;
		}
		else
			speed.Y -= 0.01f;
	}

	// Limit falling
	if (speed.Y < -4.0f)
		speed.Y = -4.0f;

	// Affect position by speed
	position += speed * deltaTime;

	// Check collision on top and bottom side
	for (s8 i = 0, j = 1; i < 2; ++i, j -= 2)
	{
		if (i && speed.Y > 0)
		{
			continue;
		}
		else if (!i && speed.Y <= 0)
		{
			continue;
		}

		// Get shifted BB
		aabbox3df rect(bbox);
		utility::getVerticalAABBox(bbox, rect, (1.0f + (0.25f * abs(speed.Y))) * j, 0.9f - abs(speed.X * 2));

		// Check for collision
		Collision collision = checkBoundingBoxCollision<Solid>(RoomManager::singleton->gameObjects, rect);
		if (collision.gameObject != nullptr)
		{
			// Cast to game object
			shared_ptr<GameObject> go = collision.getGameObject<GameObject>();

			// Play sound
			if ((!i && speed.Y > 0.1) || (i && speed.Y < -0.1))
			{
				sounds["bounce"]->play();
			}

			// Reposition object correctly
			speed.Y = 0;
			if (i)
			{
				position.Y = go->position.Y + bbox.getExtent().Y;
				falling = false;
			}
			else
			{
				position.Y = go->position.Y - collision.otherBoundingBox.getExtent().Y;
				falling = true;
				fallLine = nullptr;
			}
		}
		else if (i)
		{
			falling = true;
		}
	}

	// Check collision on left and right side
	for (s8 i = 0, j = -1; i < 2; ++i, j += 2)
	{
		if (i && speed.X <= 0)
		{
			continue;
		}
		else if (!i && speed.X >= 0)
		{
			continue;
		}

		// Get shifted BB
		aabbox3df rect(bbox);
		utility::getHorizontalAABBox(bbox, rect, (1.0f + (0.25f * abs(speed.X))) * j, 0.9f);

		// Check for collision
		Collision collision = checkBoundingBoxCollision<Solid>(RoomManager::singleton->gameObjects, rect);
		if (collision.gameObject != nullptr)
		{
			// Cast to game object
			shared_ptr<GameObject> go = collision.getGameObject<GameObject>();

			// Play sound
			if ((!i && speed.X < -0.1) || (i && speed.X > 0.1))
			{
				sounds["bounce"]->play();
			}

			// Reposition object correctly
			position.X = go->position.X;
			position.X += i ? -bbox.getExtent().X : collision.otherBoundingBox.getExtent().X;
			speed.X = 0;
		}
	}

	// Check collision with coin
	{
		aabbox3df rect(bbox);
		Collision collision = checkBoundingBoxCollision<Coin>(RoomManager::singleton->gameObjects, rect, coinCollisionCheck);
		if (collision.gameObject != nullptr)
		{
			sounds["coin"]->play();
			((Coin*)(collision.gameObject.get()))->pick();
		}
	}
}

void Player::draw()
{
	// Update model
	shared_ptr<Model> model = models.at(0);
	model->position = position;
	model->rotation = vector3df(-45.0f, 0, -position.X * 3);

	// Reposition camera
	Camera::singleton->position = position + vector3df(0, 40, -100);
	Camera::singleton->lookAt = position;
}