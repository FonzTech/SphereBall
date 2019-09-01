#include "Solid.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "RoomManager.h"
#include "Player.h"

const f32 Solid::BREAKING_THRESHOLD = 8.0f;

std::shared_ptr<Solid> Solid::createInstance(const json &jsonData)
{
	f32 breakState = -1.0F;
	try
	{
		// Check if block is breakable
		json optional = jsonData.at("optional");
		optional.at("breakState").get_to(breakState);
	}
	catch (json::exception e)
	{
	}
	return std::make_shared<Solid>(breakState);
}

Solid::Solid(const f32 breakState) : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh;

	// Load assets
	if (breakState >= 0.0f)
	{
		// Load zipped mesh
		mesh = utility::getMesh(smgr, "models/broken_block.zip");

		// Load sounds
		sounds[KEY_SOUND_BREAKING] = SoundManager::singleton->getSound(KEY_SOUND_BREAKING);
		sounds[KEY_SOUND_BREAKING]->setAttenuation(0.005f);

		sounds[KEY_SOUND_BREAK] = SoundManager::singleton->getSound(KEY_SOUND_BREAK);
		sounds[KEY_SOUND_BREAK]->setAttenuation(0.005f);
	}
	else
	{
		// Load mesh
		mesh = smgr->getMesh("models/cube.obj");
	}

	// Load texture
	ITexture* texture = driver->getTexture("textures/block.png");

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);

	// Assign members
	this->breakState = breakState;
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
}

void Solid::draw()
{
	// Update model
	if (models.size() > 0)
	{
		std::shared_ptr<Model> model = models.at(0);
		model->position = position;
	}
}

bool Solid::isSolid()
{
	return breakState < BREAKING_THRESHOLD;
}