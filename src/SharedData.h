#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#define KEY_SCORE_COIN 0
#define KEY_SCORE_KEY 1
#define KEY_SCORE_KEY_PICKED 2

#include <memory>
#include <unordered_map>

#include "EngineObject.h"

using namespace std;

class SharedData : public EngineObject
{
protected:

	// Graphics
	IGUIFont* font;

	// Structure for GUI score value
	struct ScoreValue {
		s8 visible;
		s32 value;

		ScoreValue()
		{
		}

		ScoreValue(s8 visible, s32 value)
		{
			this->visible = visible;
			this->value = value;
		}
	};

	// Map to hold textures
	unordered_map<u8, ITexture*> guiTextures;

	// Map for game scores and related data
	unordered_map<s32, ScoreValue> gameScores;

	// Build GUI from game score
	void buildGUIFromGameScore();

public:

	// Key for room loader
	static const std::string ROOM_OBJECT_KEY;

	// Singleton pattern variable
	static shared_ptr<SharedData> singleton;

	// Constructor
	void loadAssets();

	// Initialize game score value
	void initGameScoreValue(s32 key, s32 value);

	// Clear the entire game scores map
	void clearGameScore();

	// Get value for game score
	s32 getGameScoreValue(const s32 key, const s32 defaultValue = -1);

	// Update value for game score
	void updateGameScoreValue(const s32 key, const s32 stepValue);

	// Build GUI from game score
	void buildGUI();
};

#endif // SHAREDDATA_H