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
#define KEY_SOUND_CLOCK_A		"clock_a"
#define KEY_SOUND_CLOCK_B		"clock_b"
#define KEY_SOUND_TIME_OUT		"time_out"
#define KEY_SOUND_HOURGLASS		"hourglass"
#define KEY_SOUND_EXITED		"exited"
#define KEY_SOUND_FRUIT			"fruit"
#define KEY_SOUND_BREAKING		"breaking"
#define KEY_SOUND_BREAK			"break"

#define KEY_SETTING_MUSIC	1
#define KEY_SETTING_SOUND	2

#include <unordered_map>
#include <string>
#include <memory>

#include <SFML/Audio.hpp>
#include <irrlicht.h>

using namespace irr;

class SoundManager
{
public:

	SoundManager();

	// Map to hold volume levels
	std::unordered_map<u8, f32> volumeLevels;

	// Singleton holder
	static std::shared_ptr<SoundManager> singleton;

	// Map to hold sound buffers
	std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundBuffers;

	// Method to load sound buffers
	std::shared_ptr<sf::SoundBuffer> getSoundBuffer(const std::string& fname);

	// Method to obtain playable sound
	std::shared_ptr<sf::Sound> getSound(const std::string& fname);

	// Step volume level, preventing going outside bounds
	void updateVolumeLevel(const bool isMusic, const f32 stepValue);
};

#endif // SOUNDMANAGER_H