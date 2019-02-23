#include "SoundManager.h"

shared_ptr<SoundManager> SoundManager::singleton = nullptr;

shared_ptr<sf::SoundBuffer> SoundManager::getSoundBuffer(const std::string& fname)
{
	// Check if sound buffer has been already loaded
	if (soundBuffers.find(fname) != soundBuffers.end())
	{
		printf("SoundBuffer %s has been reused\n", fname.c_str());
		return soundBuffers[fname];
	}

	// Otherwise load the sound buffer into memory
	shared_ptr<sf::SoundBuffer> sb = make_shared<sf::SoundBuffer>();
	if (!sb->loadFromFile("sounds/" + fname + ".ogg"))
	{
		printf("SoundBuffer %s has not been loaded\n", fname.c_str());
		return nullptr;
	}

	// Save the new loaded sound buffer, then return it
	return soundBuffers[fname] = sb;
}

shared_ptr<sf::Sound> SoundManager::getSound(const std::string& fname)
{
	// Get sound buffer from its routine
	shared_ptr<sf::SoundBuffer> sb = this->getSoundBuffer(fname);

	// Check if sound buffer creation has encountered an error
	if (sb == nullptr)
	{
		printf("Sound %s could not be created\n", fname.c_str());
		return make_shared<sf::Sound>();
	}

	// Return the handleable sound
	return make_shared<sf::Sound>(*sb.get());
}