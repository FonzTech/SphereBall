#include "Solid.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "RoomManager.h"
#include "Player.h"

const f32 Solid::BREAKING_THRESHOLD = 8.0f;

std::shared_ptr<Solid> Solid::createInstance(const json &jsonData)
{
	f32 breakState = -1.0f;
	f32 springTension = -1.0f;

	try
	{
		// Check if block is breakable
		json optional = jsonData.at("optional");

		if (optional.find("breakState") != optional.end())
		{
			optional.at("breakState").get_to(breakState);
		}
		else if (optional.find("springTension") != optional.end())
		{
			optional.at("springTension").get_to(springTension);
		}
	}
	catch (json::exception e)
	{
	}
	return std::make_shared<Solid>(breakState, springTension);
}

Solid::Solid(const f32 breakState, const f32 springTension) : GameObject()
{
	// Declare asset variables
	IAnimatedMesh* mesh;
	ITexture* texture;

	// Load assets
	if (springTension >= 0.0f)
	{
		// Load mesh
		mesh = smgr->getMesh("models/block_spring_a.obj");

		// Load mesh for bounding box
		IMesh* bboxMesh = smgr->getMesh("models/cube.obj");
		bboxMesh->grab();
		boundingBox = bboxMesh->getBoundingBox();
		bboxMesh->drop();

		// Load texture
		texture = driver->getTexture("textures/block_spring_a.png");

		// Load sound
		sounds[KEY_SOUND_SPRING] = SoundManager::singleton->getSound(KEY_SOUND_SPRING);
	}
	else if (breakState >= 0.0f)
	{
		// Load zipped mesh
		mesh = utility::getMesh(smgr, "models/broken_block.zip");

		// Load sounds
		sounds[KEY_SOUND_BREAKING] = SoundManager::singleton->getSound(KEY_SOUND_BREAKING);
		sounds[KEY_SOUND_BREAKING]->setAttenuation(0.005f);

		sounds[KEY_SOUND_BREAK] = SoundManager::singleton->getSound(KEY_SOUND_BREAK);
		sounds[KEY_SOUND_BREAK]->setAttenuation(0.005f);

		// Load mesh for bounding box
		IMesh* bboxMesh = smgr->getMesh("models/cube.obj");
		bboxMesh->grab();
		boundingBox = bboxMesh->getBoundingBox();
		bboxMesh->drop();

		// Load texture
		texture = driver->getTexture("textures/block.png");
	}
	else
	{
		// Load mesh
		mesh = smgr->getMesh("models/cube.obj");
		boundingBox = mesh->getBoundingBox();

		// Load texture
		texture = driver->getTexture("textures/block.png");
	}

	// Create model for block
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);

	// Assign members
	this->breakState = breakState;
	this->springTension = springTension;
	springAngle = 0.0f;

	// Check if block is a spring
	if (springTension >= 0.0f)
	{
		for (u8 i = 0; i < 2; ++i)
		{
			// Get right asset name
			const std::string name = i ? "block_spring_c" : "block_spring_b";

			// Load platform mesh
			mesh = smgr->getMesh(std::string("models/" + name + ".obj").c_str());
			texture = driver->getTexture(std::string("textures/" + name + ".png").c_str());

			// Create model for platform
			model = std::make_shared<Model>(mesh);
			model->addTexture(0, texture);
			model->scale = vector3df(1);
			models.push_back(model);
		}
	}
}

void Solid::update()
{
	// Check if block is breakable
	if (breakState >= 0.0f)
	{
		// Check if object is still valid
		if (models.size())
		{
			// Get first model
			const auto& model = models.at(0);

			// Check if animation if ended
			if (breakState > model->mesh->getFrameCount())
			{
				models.clear();
			}
			// Check if block is already broken
			else if (breakState >= BREAKING_THRESHOLD)
			{
				breakState += deltaTime * 0.25f;
			}
			// Execute behaviour
			else
			{
				// Check collision on top against player
				const aabbox3df bbox = models.at(0)->mesh->getBoundingBox();
				aabbox3df rect(bbox);
				utility::getVerticalAABBox(bbox, rect, 1.0f, 0.05f);

				Collision collision = checkBoundingBoxCollision<Player>(RoomManager::singleton->gameObjects, rect);
				if (collision.engineObject != nullptr)
				{
					// Increment state
					breakState += 0.02f * deltaTime;

					// Play sound on threshold
					if (breakState >= BREAKING_THRESHOLD)
					{
						// Stop "breaking" sound
						sounds[KEY_SOUND_BREAKING]->stop();

						// Play "break" sound
						playAudio(KEY_SOUND_BREAK, &position);
					}
					// Play sound continuously
					else if (sounds[KEY_SOUND_BREAKING]->getStatus() != sf::SoundSource::Status::Playing)
					{
						playAudio(KEY_SOUND_BREAKING, &position);
					}
				}
				else
				{
					// Stop "breaking" sound
					sounds[KEY_SOUND_BREAKING]->stop();
				}
			}

			// Increment frame for the animation
			model->currentFrame = breakState;
		}
		// Destroy object when its behaviour has ended
		else if (sounds[KEY_SOUND_BREAK]->getStatus() != sf::SoundSource::Status::Playing)
		{
			destroy = true;
		}
	}

	// Check if this is a spring
	if (springTension >= 0.0f)
	{
		// Compute total angle for spring animation
		const f32 totalSpringAngle = 30.0f + 60.0f * springTension;

		// Compute collision bbox
		const aabbox3df bbox = getBoundingBox();
		aabbox3df rect(bbox);
		utility::getVerticalAABBox(bbox, rect, 1.1f);

		// Check collision against player
		Collision collision = checkBoundingBoxCollision<Player>(RoomManager::singleton->gameObjects, rect);
		if (collision.engineObject != nullptr)
		{
			// Cast to player object
			const std::shared_ptr<Player> player = collision.getGameObject<Player>();

			// Adjust spring tension effect based on player position
			const f32 diff = std::max(std::abs(player->position.X - position.X) - 10.0f, 0.0f) / 10.0f;
			springAngle = diff * totalSpringAngle;

			// Make player jump when angle is right
			if (!player->getJumpingState() && diff <= 0.0f)
			{
				// Add motion to player
				player->addSpeed(vector3df(0, 0.4f + 0.2f * springTension, 0), position + vector3df(0, 10, 0));

				// Play sound
				playAudio(KEY_SOUND_SPRING);
			}
		}
		else
		{
			springAngle += 0.25f * deltaTime;
			if (springAngle >= totalSpringAngle)
			{
				springAngle = totalSpringAngle;
			}
		}
	}
}

void Solid::draw()
{
	// Update model
	if (models.size() >= 1)
	{
		std::shared_ptr<Model> model = models.at(0);
		model->position = position;
	}

	// Update spring model
	if (models.size() >= 2)
	{
		const f32 factor = std::sin(degToRad(springAngle));

		std::shared_ptr<Model> model = models.at(1);
		model->position = position + vector3df(0, 8 + factor * 4, 0);

		model = models.at(2);
		model->position = position + vector3df(0, -9.5f, 0);
		model->scale = vector3df(1, 1.0f + factor * 0.2f, 1);
	}
}

aabbox3df Solid::getBoundingBox()
{
	return boundingBox;
}

bool Solid::isSolid()
{
	return breakState < BREAKING_THRESHOLD;
}