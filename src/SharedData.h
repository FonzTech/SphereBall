#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#define KEY_GUI_COIN		0
#define KEY_GUI_KEY			1
#define KEY_GUI_KEY_PICKED	2
#define KEY_GUI_RECTANGLE	3

#include <memory>
#include <unordered_map>

#include "EngineObject.h"

using namespace std;

class SharedData : public EngineObject
{
protected:

	// Game Over screen
	f32 gameOver = 0.0f;

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
	void buildGameScore();

	// Build GUI for Game Over screen
	void buildGameOver();

public:

	// Key for room loader
	static const std::string ROOM_OBJECT_KEY;

	// Constructor
	SharedData();

	// Singleton pattern variable
	static shared_ptr<SharedData> singleton;

	// Animation stepper
	void stepAnimations(f32 deltaTime);

	// Post Constructor
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

	// Display game over GUI menu
	void displayGameOver();
};

#endif // SHAREDDATA_H