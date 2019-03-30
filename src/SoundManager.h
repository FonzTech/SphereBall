#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#define KEY_SOUND_SELECT		"select"
#define KEY_SOUND_BOUNCE		"bounce"
#define KEY_SOUND_COIN			"coin"
#define KEY_SOUND_KEY			"key"
#define KEY_SOUND_KEY_FINAL		"key_final"
#define KEY_SOUND_SPIKE_IN		"spike_in"
#define KEY_SOUND_SPIKE_OUT		"spike_out"
#define KEY_SOUND_NAILED		"nailed"
#define KEY_SOUND_GAME_OVER		"game_over"
#define KEY_SOUND_LETHARGY_PILL	"lethargy_pill"
#define KEY_SOUND_LEVEL_START	"level_start"

#include <unordered_map>
#include <string>
#include <memory>
#include <SFML/Audio.hpp>

class SoundManager
{
public:
	// Singleton holder
	static std::shared_ptr<SoundManager> singleton;

	// Map to hold sound buffers
	std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundBuffers;

	// Method to load sound buffers
	std::shared_ptr<sf::SoundBuffer> getSoundBuffer(const std::string& fname);

	// Method to obtain playable sound
	std::shared_ptr<sf::Sound> getSound(const std::string& fname);
};

#endif // SOUNDMANAGER_H