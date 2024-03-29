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
#include "Fire.h"
#include "Teleporter.h"

const f32 Player::breathingDelta = 0.125f;

std::shared_ptr<Player> Player::createInstance(const nlohmann::json &jsonData)
{
	return std::make_shared<Player>();
}

Player::Player() : GameObject()
{
	// Load custom shader for player
	SpecializedShaderCallback* ssc = new SpecializedShaderCallback(this);

	IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	customMaterial = gpu->addHighLevelShaderMaterialFromFiles("shaders/standard.vs", "shaders/player.fs", ssc, EMT_TRANSPARENT_VERTEX_ALPHA);

	ssc->drop();

	// Load model for player
	IAnimatedMesh* mesh = smgr->getMesh("models/sphere.obj");
	ITexture* texture = driver->getTexture("textures/player.png");

	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = customMaterial;
	models.push_back(model);

	// Load model for electric effect
	{
		texture = driver->getTexture("textures/player_electric.png");

		model = std::make_shared<Model>(mesh);
		model->addTexture(0, texture);
		model->material = EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		models.push_back(model);
	}

	// Initialize variables
	state = STATE_WALKING;
	noiseFactor = 0.0f;
	cameraDistance = vector3df(0, 40, -100);

	direction = 1;
	moving = 0;
	falling = 0;
	breathing = 0.0f;
	breathingSpeed = 0.001f;

	fireFactor = 0.0f;

	playerScale = 1.0f;

	fallLine = nullptr;
	dieAlarm = nullptr;

	// Load sounds
	sounds[KEY_SOUND_BOUNCE] = SoundManager::singleton->getSound(KEY_SOUND_BOUNCE);
	sounds[KEY_SOUND_NAILED] = SoundManager::singleton->getSound(KEY_SOUND_NAILED);
	sounds[KEY_SOUND_GAME_OVER] = SoundManager::singleton->getSound(KEY_SOUND_GAME_OVER);
	sounds[KEY_SOUND_LEVEL_START] = SoundManager::singleton->getSound(KEY_SOUND_LEVEL_START);
	sounds[KEY_SOUND_EXITED] = SoundManager::singleton->getSound(KEY_SOUND_EXITED);
	sounds[KEY_SOUND_TELEPORT] = SoundManager::singleton->getSound(KEY_SOUND_TELEPORT);

	sounds[KEY_SOUND_COIN] = SoundManager::singleton->getSound(KEY_SOUND_COIN);
	sounds[KEY_SOUND_KEY] = SoundManager::singleton->getSound(KEY_SOUND_KEY);
	sounds[KEY_SOUND_KEY_FINAL] = SoundManager::singleton->getSound(KEY_SOUND_KEY_FINAL);
	sounds[KEY_SOUND_LETHARGY_PILL] = SoundManager::singleton->getSound(KEY_SOUND_LETHARGY_PILL);
	sounds[KEY_SOUND_HOURGLASS] = SoundManager::singleton->getSound(KEY_SOUND_HOURGLASS);
	sounds[KEY_SOUND_FRUIT] = SoundManager::singleton->getSound(KEY_SOUND_FRUIT);

	// Create specialized functions
	collisionChecks["solid"] = [](GameObject* go)
	{
		return ((Solid*)go)->isSolid();
	};

	collisionChecks["solidTop"] = [this](GameObject* go)
	{
		Solid* solid = (Solid*)go;
		return solid->position.Y + 1.0f < position.Y && solid->isSolid();
	};

	collisionChecks["pickup"] = [](GameObject* go)
	{
		return ((Pickup*)go)->notPicked;
	};

	collisionChecks["spikes"] = [](GameObject* go)
	{
		Spikes* spikes = (Spikes*)go;
		return spikes->isHarmful();
	};

	// Fade out
	SharedData::singleton->startFade(false, nullptr);

	// Play Start Level sound
	playAudio(KEY_SOUND_LEVEL_START, nullptr);
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

		// Check for time out
		if (SharedData::singleton->hasLevelTimedOut())
		{
			// Load noise texture
			ITexture* texture = driver->getTexture("textures/noise_128.png");
			models.at(0)->addTexture(1, texture);

			// Switch state
			state = STATE_TIME_OUT;

			// Trigger die alarm
			dieAlarm = std::make_unique<Alarm>(2000.0f);
		}
		// Check for fell off
		else if (position.Y < RoomManager::singleton->lowerBound)
		{
			// Switch state
			state = STATE_FELLOFF;

			// Trigger die alarm
			dieAlarm = std::make_unique<Alarm>(1500.0f);
		}
	}
	else if (state == STATE_DEAD)
	{
		// Play behaviour
		dead();
	}
	else if (state == STATE_EXITED)
	{
		// Reset breathing
		resetBreathing();

		// Execute behaviour
		exited();
	}
	else if (state == STATE_TIME_OUT)
	{
		// Reset breathing
		resetBreathing();

		// Increment noise factor
		noiseFactor += 0.00075f * deltaTime;

		// Play behaviour
		dead();
	}
	else if (state == STATE_BURNED)
	{
		// Reset breathing
		resetBreathing();
	}
	else if (state == STATE_FELLOFF)
	{
		// Apply physics to player, but still play "dead" behaviour
		walk();
		dead();
	}
	
	if (state == STATE_TELEPORT)
	{
		// Snap to warper position
		position += (warpingTeleporter - position) / 4;

		// Warping effect
		playerScale -= 0.002f * deltaTime;
		if (playerScale < 0.0f)
		{
			playerScale = 0.0f;
		}

		// Control fade in 
		SharedData::singleton->fadeValue = 1.0f - playerScale;

		// Warping timer
		teleportAlarm->stepDecrement(deltaTime);
		if (teleportAlarm->isTriggered())
		{
			// Make electric ball invisible
			models.at(1)->scale = vector3df(0);

			// Restore player
			position = warpingPosition;
			state = STATE_WALKING;

			// Start fade out
			SharedData::singleton->startFade(false, nullptr);

			// Delete alarm
			teleportAlarm = nullptr;
		}
	}
	else
	{
		// Reverse warping effect
		playerScale += 0.005f * deltaTime;
		if (playerScale > 1.0f)
		{
			playerScale = 1.0f;
		}
	}

	// Update listener position
	// sf::Listener::setDirection(sf::Vector3f(0, 0, -1));
	// sf::Listener::setPosition(Utility::irrVectorToSf(position));
}

void Player::draw()
{
	// Update model
	if (models.size() > 0)
	{
		// Get main model
		std::shared_ptr<Model> & model = models.at(0);

		if (state == STATE_DEAD)
		{
			std::shared_ptr<Model> model = models.at(0);
			model->position = position + vector3df(0, 0, -11);
		}
		else // if (state == STATE_WALKING)
		{
			// Update model parameters
			model->position = position;
			model->rotation = vector3df(0);

			// Update matrix for shader
			updateTransformMatrix();

			// Set other models parameters
			std::shared_ptr<Model> & model2 = models.at(1);

			if (state == STATE_TELEPORT)
			{
				f32 time = (f32)device->getTimer()->getTime();
				model2->rotation = vector3df(0, 0, std::floorf(time / 40.0f) * 90.0f);
				model2->position = position;
			}
		}
	}

	// Reposition camera
	if (state != STATE_FELLOFF)
	{
		Camera::singleton->setPosition(position + cameraDistance);
	}
	Camera::singleton->setLookAt(position);
}

s32 Player::getJumpingState()
{
	if (fallLine != nullptr)
	{
		if (speed.Y > 0.0f)
		{
			return 1;
		}
	}
	else if (speed.Y < 0.0f)
	{
		return 2;
	}
	return 0;
}

void Player::addSpeed(const vector3df & motion, const vector3df & fallLine)
{
	// Apply speed
	speed += motion;

	// Set fall line above player
	this->fallLine = std::make_unique<vector3df>(fallLine);

	// Turn on falling
	falling = 1;
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
	scaling.setScale(vector3df(playerScale, scaleHeight * playerScale, playerScale));
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
	if (state != STATE_FELLOFF)
	{
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
		fallLine = std::make_unique<vector3df>(position + vector3df(0, bbox.getExtent().Y, 0));

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

			// Limit fall speed
			if (speed.Y < -10.0f)
			{
				speed.Y = -10.0f;
			}
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
		Utility::getVerticalAABBox(bbox, rect, (1.0f + (0.25f * std::abs(speed.Y))) * j, 0.75f - std::abs(speed.X * 2));

		// Check for collision
		Collision collision = checkBoundingBoxCollision<Solid>(RoomManager::singleton->gameObjects, rect, collisionChecks[i ? "solidTop" : "solid"]);
		if (collision.engineObject != nullptr)
		{
			// Cast to game object
			std::shared_ptr<GameObject> go = collision.getGameObject<Solid>();

			// Motion effects
			if ((!i && speed.Y > 0.001) || (i && speed.Y < -0.001))
			{
				// Play sound
				playAudio(KEY_SOUND_BOUNCE);

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
		Utility::getHorizontalAABBox(bbox, rect, (0.85f + (0.1f * std::abs(speed.X))) * j, 0.9f);

		// Check for collision
		Collision collision = checkBoundingBoxCollision<Solid>(RoomManager::singleton->gameObjects, rect, collisionChecks["solid"]);
		if (collision.engineObject != nullptr)
		{
			// Cast to game object
			std::shared_ptr<GameObject> go = collision.getGameObject<GameObject>();

			// Play sound
			if ((!i && speed.X < -0.05) || (i && speed.X > 0.05))
			{
				playAudio(KEY_SOUND_BOUNCE);
			}

			// Reposition object correctly
			position.X = go->position.X;
			position.X += (i ? -bbox.getExtent().X : collision.otherBoundingBox.getExtent().X) * 0.9f;
			speed.X = 0;
		}
	}

	// Check collision with pickup
	if (state == STATE_WALKING)
	{
		aabbox3df rect(bbox);
		Collision collision = checkBoundingBoxCollision<Pickup>(RoomManager::singleton->gameObjects, rect, collisionChecks["pickup"]);
		if (collision.engineObject != nullptr)
		{
			// Trigger pick
			std::shared_ptr<Pickup> pickup = collision.getGameObject<Pickup>();
			pickup->pick();

			// Play audio
			playAudio(pickup->soundIndex);
		}
	}

	// Check collision with spikes
	if (state == STATE_WALKING)
	{
		aabbox3df rect(bbox);
		Utility::transformAABBox(rect, vector3df(0), vector3df(0), vector3df(0.75f, 0.85f, 1.0f));

		Collision collision = checkBoundingBoxCollision<Spikes>(RoomManager::singleton->gameObjects, rect, collisionChecks["spikes"]);
		if (collision.engineObject != nullptr)
		{
			playAudio(KEY_SOUND_NAILED);
			die();
		}
	}

	// Check collision with exit
	if (state == STATE_WALKING && !falling && speed.Y == 0.0f)
	{
		// Check for score
		if (SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_PICKED) == SharedData::singleton->getGameScoreValue(KEY_SCORE_KEY_TOTAL))
		{
			aabbox3df rect(bbox);
			Utility::transformAABBox(rect, vector3df(0), vector3df(0), vector3df(0.9f, 0.8f, 0.8f));

			Collision collision = checkBoundingBoxCollision<Exit>(RoomManager::singleton->gameObjects, rect);
			if (collision.engineObject != nullptr)
			{
				// Play sound
				playAudio(KEY_SOUND_EXITED);

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

	// Check collision with fire
	if (state == STATE_WALKING)
	{
		// Affect player
		aabbox3df rect(bbox);
		Collision collision = checkBoundingBoxCollision<Fire>(RoomManager::singleton->gameObjects, rect);
		if (collision.engineObject != nullptr)
		{
			// Raise fire effect
			fireFactor += 0.001f * deltaTime;

			// Check for threshold
			if (fireFactor >= 1.0f)
			{
				// Cap value
				fireFactor = 1.0f;

				// Change player state
				state = STATE_BURNED;
			}
		}
		// Lower fire effect
		else if (fireFactor > 0.0f && !falling && speed.Y == 0.0f)
		{
			fireFactor -= 0.001f * deltaTime;
			if (fireFactor < 0)
			{
				fireFactor = 0;
			}
		}
	}

	// Check collision with teleporter
	if (state == STATE_WALKING && !falling && speed == vector3df(0))
	{
		// Get shifted BB
		aabbox3df rect(bbox);
		Utility::getHorizontalAABBox(bbox, rect, 0);

		// Check collision
		Collision collision = checkBoundingBoxCollision<Teleporter>(RoomManager::singleton->gameObjects, rect);
		if (collision.engineObject != nullptr)
		{
			// Trigger alarm
			teleportAlarm = std::make_unique<Alarm>(600.0f);

			// Play sound
			playAudio(KEY_SOUND_TELEPORT);

			// Make electric ball visible
			models.at(1)->scale = vector3df(1);

			// Change player state
			std::shared_ptr<Teleporter> teleporter = collision.getGameObject<Teleporter>();
			warpingTeleporter = teleporter->position + vector3df(0, 10, 0);
			warpingPosition = teleporter->warp;
			state = STATE_TELEPORT;
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
	models.clear();

	// Load model for nailed state
	IAnimatedMesh* mesh = smgr->getMesh("models/plane.obj");
	ITexture* texture = driver->getTexture("textures/nailed.png");

	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->material = getCommonBasicMaterial(EMT_TRANSPARENT_VERTEX_ALPHA);
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
			playAudio(KEY_SOUND_GAME_OVER);

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

void Player::resetBreathing()
{
	breathing += ((f32)std::cos(-1) - breathing) * 0.005f * deltaTime;
}

Player::SpecializedShaderCallback::SpecializedShaderCallback(Player* player)
{
	this->player = player;
}

void Player::SpecializedShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	// Set custom world matrix. After restore the original one
	matrix4 etsWorld = driver->getTransform(ETS_WORLD);
	driver->setTransform(ETS_WORLD, player->transformMatrix);
	ShaderCallback::OnSetConstants(services, userData);
	driver->setTransform(ETS_WORLD, etsWorld);

	// Setup fire effect
	services->setPixelShaderConstant("fireFactor", (f32*)&this->player->fireFactor, 1);

	// Setup timeout effect
	services->setPixelShaderConstant("noiseFactor", (f32*)&this->player->noiseFactor, 1);

	if (this->player->state == STATE_TIME_OUT)
	{
		const s32 layer1 = 1;
		services->setPixelShaderConstant("noiseTexture", (s32*)&layer1, 1);
	}
}