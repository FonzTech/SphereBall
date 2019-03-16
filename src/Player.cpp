#include "Player.h"

#include "EventManager.h"
#include "RoomManager.h"
#include "SoundManager.h"
#include "SharedData.h"
#include "Camera.h"

#include "Utility.h"
#include "Solid.h"
#include "Coin.h"
#include "Key.h"
#include "Spikes.h"
#include "Pill.h"

shared_ptr<Player> Player::createInstance(const json &jsonData)
{
	return make_shared<Player>();
}

Player::Player() : GameObject()
{
	// Load model for player
	IAnimatedMesh* mesh = smgr->getMesh("models/sphere.obj");
	ITexture* texture = driver->getTexture("textures/player.png");

	shared_ptr<Model> model = make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);

	// Initialize variables
	state = STATE_WALKING;

	direction = 1;
	moving = 0;
	falling = 0;

	fallLine = nullptr;
	dieAlarm = nullptr;

	// Load sounds
	sounds[KEY_SOUND_BOUNCE] = SoundManager::singleton->getSound(KEY_SOUND_BOUNCE);
	sounds[KEY_SOUND_COIN] = SoundManager::singleton->getSound(KEY_SOUND_COIN);
	sounds[KEY_SOUND_KEY] = SoundManager::singleton->getSound(KEY_SOUND_KEY);
	sounds[KEY_SOUND_KEY_FINAL] = SoundManager::singleton->getSound(KEY_SOUND_KEY_FINAL);
	sounds[KEY_SOUND_NAILED] = SoundManager::singleton->getSound(KEY_SOUND_NAILED);
	sounds[KEY_SOUND_GAME_OVER] = SoundManager::singleton->getSound(KEY_SOUND_GAME_OVER);
	sounds[KEY_SOUND_LETHARGY_PILL] = SoundManager::singleton->getSound(KEY_SOUND_LETHARGY_PILL);

	// Create specialized functions
	coinCollisionCheck = [](const GameObject* go)
	{
		Coin* coin = (Coin*)go;
		return coin->notPicked;
	};

	spikesCollisionCheck = [](const GameObject* go)
	{
		Spikes* spikes = (Spikes*)go;
		return spikes->isHarmful();
	};
}

void Player::update()
{
	// Execute code for matching player state
	if (state == STATE_WALKING)
	{
		walk();
	}
	else if (state == STATE_DEAD)
	{
		dead();
	}

	// Update listener position
	sf::Listener::setDirection(sf::Vector3f(0, 0, -1));
	sf::Listener::setPosition(utility::irrVectorToSf(position));
}

void Player::draw()
{
	// Update model
	if (models.size() > 0)
	{
		if (state == STATE_WALKING)
		{
			shared_ptr<Model> model = models.at(0);
			model->position = position;
			model->rotation = vector3df(-45.0f, 0, -position.X * 3);
		}
		else if (state == STATE_DEAD)
		{
			shared_ptr<Model> model = models.at(0);
			model->position = position + vector3df(0, 0, -11);
		}
	}

	// Reposition camera
	Camera::singleton->position = position + vector3df(0, 40, -100);
	Camera::singleton->lookAt = position;
}

void Player::walk()
{
	// Get main model bounding box
	aabbox3df bbox = models.at(0)->mesh->getBoundingBox();

	// Make horizontal movements
	if (EventManager::singleton->keyStates[KEY_LEFT] >= KEY_PRESSED)
	{
		speed += vector3df(-0.01f, 0, 0);
		moving = 1;
	}
	else if (EventManager::singleton->keyStates[KEY_RIGHT] >= KEY_PRESSED)
	{
		speed += vector3df(0.01f, 0, 0);
		moving = 1;
	}
	else
	{
		moving = 0;
	}

	// Check for direction
	if (speed.X < 0)
	{
		direction = -1;
	}
	else if (speed.X > 0)
	{
		direction = 1;
	}

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
	{
		speed.X = -0.1f;
	}
	else if (speed.X > 0.1f)
	{
		speed.X = 0.1f;
	}

	// Check for input
	if (!falling && EventManager::singleton->keyStates[KEY_UP] == KEY_PRESSED)
	{
		// Setup vertical speed
		speed += vector3df(0, 0.2f, 0);

		// Set fall line above player
		vector3df fl(position + vector3df(0, bbox.getExtent().Y, 0));
		fallLine = unique_ptr<vector3df>(new vector3df(fl));

		// Turn on falling
		falling = 1;
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
		{
			speed.Y -= 0.01f;
		}
	}

	// Limit falling
	if (speed.Y < -4.0f)
	{
		speed.Y = -4.0f;
	}

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
		if (collision.engineObject != nullptr)
		{
			// Cast to game object
			shared_ptr<GameObject> go = collision.getGameObject<Solid>();

			// Play sound
			if ((!i && speed.Y > 0.1) || (i && speed.Y < -0.1))
			{
				playSound(KEY_SOUND_BOUNCE);
			}

			// Reposition object correctly
			speed.Y = 0;
			if (i)
			{
				position.Y = go->position.Y + bbox.getExtent().Y;
				falling = 0;
			}
			else
			{
				position.Y = go->position.Y - collision.otherBoundingBox.getExtent().Y;
				falling = 1;
				fallLine = nullptr;
			}
		}
		else if (i)
		{
			falling = 1;
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
		if (collision.engineObject != nullptr)
		{
			// Cast to game object
			shared_ptr<GameObject> go = collision.getGameObject<GameObject>();

			// Play sound
			if ((!i && speed.X < -0.1) || (i && speed.X > 0.1))
			{
				playSound(KEY_SOUND_BOUNCE);
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
		if (collision.engineObject != nullptr)
		{
			playSound(KEY_SOUND_COIN, &collision.getGameObject<Coin>()->position);
			collision.getGameObject<Coin>()->pick();
		}
	}

	// Check collision with key
	{
		aabbox3df rect(bbox);
		Collision collision = checkBoundingBoxCollision<Key>(RoomManager::singleton->gameObjects, rect);
		if (collision.engineObject != nullptr)
		{
			std::string soundToPlay = collision.getGameObject<Key>()->pick() ? KEY_SOUND_KEY_FINAL : KEY_SOUND_KEY;
			playSound(soundToPlay, &collision.getGameObject<Key>()->position);
		}
	}

	// Check collision with spikes
	{
		aabbox3df rect(bbox);
		utility::transformAABBox(rect, vector3df(0), vector3df(0), vector3df(0.75f, 0.85f, 1.0f));

		Collision collision = checkBoundingBoxCollision<Spikes>(RoomManager::singleton->gameObjects, rect, spikesCollisionCheck);
		if (collision.engineObject != nullptr)
		{
			playSound(KEY_SOUND_NAILED);
			die();
		}
	}

	// Check collision with pill
	{
		aabbox3df rect(bbox);
		utility::transformAABBox(rect, vector3df(0), vector3df(0), vector3df(0.8f, 0.8f, 0.8f));

		Collision collision = checkBoundingBoxCollision<Pill>(RoomManager::singleton->gameObjects, rect);
		if (collision.engineObject != nullptr)
		{
			playSound(KEY_SOUND_LETHARGY_PILL);
			collision.getGameObject<Pill>()->destroy = 1;
		}
	}
}

void Player::die()
{
	// Set dead state
	state = STATE_DEAD;

	// Trigger die alarm
	dieAlarm = make_unique<Alarm>(1500.0f);

	// Erase sphere model
	models.erase(models.begin());

	// Load model for nailed state
	IAnimatedMesh* mesh = smgr->getMesh("models/plane.obj");
	ITexture* texture = driver->getTexture("textures/nailed.png");

	shared_ptr<Model> model = make_shared<Model>(mesh);
	model->material = EMT_TRANSPARENT_ALPHA_CHANNEL;
	model->addTexture(0, texture);
	models.push_back(model);

	// Create alarm for pop animation
	popAlarm = make_unique<Alarm>(50.0f);
}

void Player::dead()
{
	// Alarm for waiting
	if (dieAlarm != nullptr)
	{
		dieAlarm->stepDecrement(deltaTime);

		// Check for alarm to be finished
		if (dieAlarm->isTriggered())
		{
			// Delete alarm
			dieAlarm = nullptr;

			// Play game over sound
			playSound(KEY_SOUND_GAME_OVER);

			// Display game over GUI menu
			SharedData::singleton->displayGameOver();
		}
	}

	// Alarm for pop animation
	if (popAlarm != nullptr)
	{
		popAlarm->stepDecrement(deltaTime);

		// Check for alarm to be finished
		if (popAlarm->isTriggered())
		{
			// Scale to achieve desired animation
			vector3df* scale = &models.at(0)->scale;

			// Check if animation should end
			if (scale->X > 2.0f)
			{
				// Delete model
				models.erase(models.begin());

				// Delete alarm
				popAlarm = nullptr;
			}
			else
			{
				const f32 s = scale->X + 0.5f;
				const f32 y = scale->Y > 0.0f ? 0.0f : s;
				*scale = vector3df(s, y, s);

				// Restart alarm
				popAlarm->setTime(50.0f);
			}
		}
	}
}