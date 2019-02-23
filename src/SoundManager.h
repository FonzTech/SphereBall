#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#define KEY_SOUND_SELECT		"select"
#define KEY_SOUND_BOUNCE		"bounce"
#define KEY_SOUND_COIN			"coin"
#define KEY_SOUND_KEY			"key"
#define KEY_SOUND_KEY_FINAL		"key_final"
#define KEY_SOUND_SPIKE_IN		"spike_in"
#define KEY_SOUND_SPIKE_OUT		"spike_out"

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