#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <SFML/Audio.hpp>

using namespace std;

class SoundManager
{
public:
	// Singleton holder
	static shared_ptr<SoundManager> singleton;

	// Map to hold sound buffers
	std::unordered_map<std::string, shared_ptr<sf::SoundBuffer>> soundBuffers;

	// Method to load sound buffers
	shared_ptr<sf::SoundBuffer> getSoundBuffer(const std::string& fname);

	// Method to obtain playable sound
	shared_ptr<sf::Sound> getSound(const std::string& fname);
};

#endif // SOUNDMANAGER_H