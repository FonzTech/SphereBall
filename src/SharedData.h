#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#define KEY_GUI_COIN			0
#define KEY_GUI_KEY				1
#define KEY_GUI_KEY_PICKED		2
#define KEY_GUI_RECTANGLE		3
#define KEY_GUI_MOUSE			4
#define KEY_GUI_HOURGLASS		5
#define KEY_GUI_HOURGLASS_SAND	6

#define KEY_TEXT_GAME_OVER	0

#define KEY_PP_WAVE	0

#include <nlohmann/json.hpp>

#include <memory>
#include <unordered_map>
#include <functional>

#include "EngineObject.h"

using nlohmann::json;

class SharedData : public EngineObject
{
protected:

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

	// Game Over screen
	f32 gameOverAlpha;
	s8 gameOverSelection;
	std::vector<recti> gameOverRects;

	// Variables for fade transition
	s8 fadeType;
	f32 fadeValue;
	std::function<void(void)> fadeCallback;

	// Font for GUI
	IGUIFont* font;

	// Map to hold render target textures
	std::unordered_map<u8, ITexture*> renderTargetTextures;

	// Map to hold post processing effect
	std::unordered_map<u8, std::function<void(const json&)>> ppCallbacks;

	// Map to hold textures
	std::unordered_map<u8, ITexture*> guiTextures;

	// Map to hold text groups
	std::unordered_map<u8, std::vector<std::wstring>> textGroups;

	// Map for game scores and related data
	std::unordered_map<s32, ScoreValue> gameScores;

	// Build fade for transition
	void buildFadeTransition();

	// Build GUI from game score
	void buildGameScore();

	// Build GUI for Game Over screen
	void buildGameOver();

	// Restart room method
	void restartRoom();

	// Back to menu room method
	void jumpToMenuRoom();

public:

	// Key for room loader
	static const std::string ROOM_OBJECT_KEY;

	// Constructor
	SharedData();

	// Singleton pattern variable
	static std::shared_ptr<SharedData> singleton;

	// Animation stepper
	void update(f32 deltaTime);

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

	// Fade control
	void startFade(bool in, std::function<void(void)> fadeCallback, f32 value);

	void startFade(bool in, std::function<void(void)> fadeCallback);

	// Build GUI from game score
	void buildGUI();

	// Display game over GUI menu
	void displayGameOver();

	// Dispose unneeded resource at frame end
	void disposeResourcesAtFrameEnd();

	/*
		Set function for post processing effect.
		Supply nullptr for callback to erase the desired key.
	*/
	void setPostProcessingCallback(u8 key, std::function<void(const json&)> callback);

	// Trigger post processing callback
	bool triggerPostProcessingCallback(u8 key, const json& data);
};

#endif // SHAREDDATA_H