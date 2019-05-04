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
#include "Hourglass.h"
#include "Exit.h"

const f32 Player::breathingDelta = 0.125f;

std::shared_ptr<Player> Player::createInstance(const json &jsonData)
{
	return std::make_shared<Player>();
}

Player::Player() : GameObject()
{
	// Load custom shader for player
	SpecializedShaderCallback* ssc = new SpecializedShaderCallback(this);

	IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	customMaterial = gpu->addHighLevelShaderMaterialFromFiles("shaders/standard.vs", "shaders/player.fs", ssc);

	ssc->drop();

	// Load model for player
	IAnimatedMesh* mesh = smgr->getMesh("models/sphere.obj");
	ITexture* texture = driver->getTexture("textures/player.png");

	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = customMaterial;
	models.push_back(model);

	// Initialize variables
	state = STATE_WALKING;

	direction = 1;
	moving = 0;
	falling = 0;
	breathing = 0.0f;
	breathingSpeed = 0.001f;

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
	sounds[KEY_SOUND_LEVEL_START] = SoundManager::singleton->getSound(KEY_SOUND_LEVEL_START);
	sounds[KEY_SOUND_HOURGLASS] = SoundManager::singleton->getSound(KEY_SOUND_HOURGLASS);
	sounds[KEY_SOUND_EXITED] = SoundManager::singleton->getSound(KEY_SOUND_EXITED);

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

	// Fade out
	SharedData::singleton->startFade(false, nullptr);

	// Play Start Level sound
	playSound(KEY_SOUND_LEVEL_START, nullptr);
}

void Player::update()
{
	// Execute code for matching player state
	if (state == STATE_WALKING)
	{
		// Decrease breathing speed
		breathingSpeed -= 0.00005f * deltaTime;

		// Limit breathing speed
		if (breathingSpeed <= 0.001f)
		{
			breathingSpeed = 0.001f;
		}

		breathing = std::fmod(breathing + breathingSpeed * deltaTime, (f32)std::acos(-1) * 2.0f);

		// Walk code
		walk();
	}
	else if (state == STATE_DEAD)
	{
		dead();
	}
	else if (state == STATE_EXITED)
	{
		// Reset breathing
		breathing += ((f32)std::cos(-1) - breathing) * 0.005f * deltaTime;

		// Execute behaviour
		exited();
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
		if (state == STATE_DEAD)
		{
			std::shared_ptr<Model> model = models.at(0);
			model->position = position + vector3df(0, 0, -11);
		}
		else // if (state == STATE_WALKING)
		{
			// Update matrix for shader
			updateTransformMatrix();

			// Update model parameters
			std::shared_ptr<Model> model = models.at(0);
			model->position = vector3df(0);
			model->rotation = vector3df(0);
		}
	}

	// Reposition camera
	Camera::singleton->position = position + vector3df(0, 40, -100);
	Camera::singleton->lookAt = position;
}

void Player::updateTransformMatrix()
{
	// Get required values
	f32 height = models.at(0)->mesh->getBoundingBox().getExtent().Y;
	f32 phaseAngle = (f32)std::sin(breathing);

	f32 scaleHeight = phaseAngle * breathingDelta + (1.0f - breathingDelta);
	f32 translateHeight = height * -breathingDelta + (phaseAngle * 0.5f + 0.5f) * height * breathingDelta;

	// Rotation
	matrix4 rotation;
	rotation.setRotationDegrees(vector3df(-45.0f, 0.0f, -position.X * 3.0f));

	// Scaling
	matrix4 scaling;
	scaling.setScale(vector3df(1.0f, scaleHeight, 1.0f));
	scaling.setTranslation(vector3df(0.0f, translateHeight, 0.0f));

	// Translation
	matrix4 translation;
	translation.setTranslation(position);

	// Final matrix
	transformMatrix = translation * scaling * rotation;
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
			{
				speed.X = 0;
			}
		}
		// Right
		else
		{
			speed += vector3df(-0.01f, 0, 0);
			if (speed.X < 0)
			{
				speed.X = 0;
			}
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
		fallLine = std::unique_ptr<vector3df>(new vector3df(fl));

		// Turn on falling
		falling = 1;
	}

	// Gravity
	if (falling)
	{
		if (fallLine != nullptr)
		{
			if (position.Y >= fallLine->Y)
			{
				fallLine = nullptr;
			}
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
			std::shared_ptr<GameObject> go = collision.getGameObject<Solid>();

			// Motion effects
			if ((!i && speed.Y > 0.1) || (i && speed.Y < -0.1))
			{
				// Play sound
				playSound(KEY_SOUND_BOUNCE);

				// Reset breathing
				breathingSpeed = 0.025f;
			}

			// Stop vertical movement
			speed.Y = 0;

			// Reposition object correctly
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
			std::shared_ptr<GameObject> go = collision.getGameObject<GameObject>();

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
	if (state == STATE_WALKING)
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
	if (state == STATE_WALKING)
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
	if (state == STATE_WALKING)
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
	if (state == STATE_WALKING)
	{
		aabbox3df rect(bbox);
		utility::transformAABBox(rect, vector3df(0), vector3df(0), vector3df(0.9f, 0.8f, 0.8f));

		Collision collision = checkBoundingBoxCollision<Pill>(RoomManager::singleton->gameObjects, rect);
		if (collision.engineObject != nullptr)
		{
			// Play sound
			playSound(KEY_SOUND_LETHARGY_PILL);

			// Destroy object
			collision.getGameObject<Pill>()->pick();

			// Trigger wave effect
			json data;
			data["speed"] = 0.00001f;
			data["strength"] = 0.05f;
			SharedData::singleton->triggerPostProcessingCallback(KEY_PP_WAVE, data);

			// Trigger fade out
			SharedData::singleton->startFade(false, nullptr, 1.0f);
		}
	}

	// Check collision with hourglass
	if (state == STATE_WALKING)
	{
		aabbox3df rect(bbox);
		utility::transformAABBox(rect, vector3df(0), vector3df(0), vector3df(0.9f, 0.8f, 0.8f));

		Collision collision = checkBoundingBoxCollision<Hourglass>(RoomManager::singleton->gameObjects, rect);
		if (collision.engineObject != nullptr)
		{
			// Play sound
			playSound(KEY_SOUND_HOURGLASS);

			// Destroy object
			collision.getGameObject<Hourglass>()->pick();

			// Invert time
			SharedData::singleton->invertTime();
		}
	}

	// Check collision with exit
	if (state == STATE_WALKING && !falling && speed.Y == 0.0f)
	{
		// Check for score
		if (SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_PICKED) == SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_TOTAL))
		{
			aabbox3df rect(bbox);
			utility::transformAABBox(rect, vector3df(0), vector3df(0), vector3df(0.9f, 0.8f, 0.8f));

			Collision collision = checkBoundingBoxCollision<Exit>(RoomManager::singleton->gameObjects, rect);
			if (collision.engineObject != nullptr)
			{
				// Play sound
				playSound(KEY_SOUND_EXITED);

				// Stop time
				SharedData::singleton->displayExit();

				// Change state
				state = STATE_EXITED;

				std::shared_ptr<Exit> exit = collision.getGameObject<Exit>();
				exitedPosition = exit->position;
				exit->fade();
			}
		}
	}
}

void Player::die()
{
	// Set dead state
	state = STATE_DEAD;

	// Trigger die alarm
	dieAlarm = std::make_unique<Alarm>(1500.0f);

	// Erase sphere model
	models.erase(models.begin());

	// Load model for nailed state
	IAnimatedMesh* mesh = smgr->getMesh("models/plane.obj");
	ITexture* texture = driver->getTexture("textures/nailed.png");

	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->material = EMT_TRANSPARENT_ALPHA_CHANNEL;
	model->addTexture(0, texture);
	models.push_back(model);

	// Create alarm for pop animation
	popAlarm = std::make_unique<Alarm>(50.0f);
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
			SharedData::singleton->displayLevelEnd();
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

void Player::exited()
{
	if (position.X >= exitedPosition.X - 1.0f && position.X <= exitedPosition.X + 1.0f)
	{
		// Fixed horizontal position
		position.X = exitedPosition.X;

		// Move vertically down
		position.Y -= 0.05f * deltaTime;
		if (position.Y <= exitedPosition.Y - 24)
		{
			position.Y = exitedPosition.Y - 24;
		}
	}
	else
	{
		f32 direction = position.X < exitedPosition.X ? 1.0f : -1.0f;
		position += vector3df(0.05f * deltaTime, 0, 0) * direction;
	}

	position.Z = 0.5f;
}

Player::SpecializedShaderCallback::SpecializedShaderCallback(Player* player)
{
	this->player = player;
}

void Player::SpecializedShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Set custom world matrix
	driver->setTransform(ETS_WORLD, player->transformMatrix);

	// Execute parent method
	ShaderCallback::OnSetConstants(services, userData);

	// Restore world matrix
	driver->setTransform(ETS_WORLD, IdentityMatrix);
}