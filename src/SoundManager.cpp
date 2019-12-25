#include "SoundManager.h"

std::shared_ptr<SoundManager> SoundManager::singleton = nullptr;

SoundManager::SoundManager()
{
	volumeLevels[KEY_SETTING_MUSIC] = 20.0f;
	volumeLevels[KEY_SETTING_SOUND] = 20.0f;
}

std::shared_ptr<sf::SoundBuffer> SoundManager::getSoundBuffer(const std::string& fname)
{
	// Check if sound buffer has been already loaded
	const auto& iterator = soundBuffers.find(fname);
	if (iterator != soundBuffers.end())
	{
		#if NDEBUG || _DEBUG
		printf("SoundBuffer %s has been reused\n", fname.c_str());
		#endif

		return iterator->second;
	}

	// Otherwise load the sound buffer into memory
	std::shared_ptr<sf::SoundBuffer> sb = std::make_shared<sf::SoundBuffer>();
	if (!sb->loadFromFile("sounds/" + fname + ".ogg"))
	{
		#if NDEBUG || _DEBUG
		printf("SoundBuffer %s has NOT been loaded\n", fname.c_str());
		#endif

		return nullptr;
	}

	// Save the new loaded sound buffer, then return it
	return soundBuffers[fname] = sb;
}

std::shared_ptr<sf::Sound> SoundManager::getSound(const std::string& fname)
{
	// Get sound buffer from its routine
	std::shared_ptr<sf::SoundBuffer> sb = this->getSoundBuffer(fname);

	// Check if sound buffer creation has encountered an error
	if (sb == nullptr)
	{
		#if NDEBUG || _DEBUG
		printf("Sound %s could NOT be created\n", fname.c_str());
		#endif

		return std::make_shared<sf::Sound>();
	}

	// Return the handleable sound
	return std::make_shared<sf::Sound>(*sb.get());
}

void SoundManager::updateVolumeLevel(const bool isMusic, const f32 stepValue)
{
	f32* value = &volumeLevels[isMusic ? KEY_SETTING_MUSIC : KEY_SETTING_SOUND];
	*value = std::max(0.0f, std::min(*value + stepValue, 100.0f));
}