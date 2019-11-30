#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#define KEY_SCORE_COIN			0
#define KEY_SCORE_KEY_PICKED	1
#define KEY_SCORE_KEY_TOTAL		2
#define KEY_SCORE_TIME			3
#define KEY_SCORE_TIME_MAX		4
#define KEY_SCORE_ITEMS_PICKED	5
#define KEY_SCORE_ITEMS_MAX		6
#define KEY_SCORE_POINTS		7
#define KEY_SCORE_POINTS_TOTAL	8
#define KEY_SCORE_FRUITS		9

#define KEY_GUI_COIN					0
#define KEY_GUI_KEY						1
#define KEY_GUI_KEY_PICKED				2
#define KEY_GUI_RECTANGLE				3
#define KEY_GUI_MOUSE					4
#define KEY_GUI_HOURGLASS				5
#define KEY_GUI_HOURGLASS_SAND_TOP		6
#define KEY_GUI_HOURGLASS_SAND_BOTTOM	7
#define KEY_GUI_APPLE					9
#define KEY_GUI_BANANA					10
#define KEY_GUI_STRAWBERRY				11
#define KEY_GUI_WATERMELON				12
#define KEY_GUI_PINEAPPLE				13

#define KEY_TEXT_GAME_OVER		0
#define KEY_TEXT_LEVEL_PASSED	1

#define KEY_PP_WAVE		0
#define KEY_PP_RIPPLE	1
#define KEY_PP_BLUR		2

#include <nlohmann/json.hpp>

#include <memory>
#include <unordered_map>
#include <functional>

#include "EngineObject.h"
#include "Alarm.h"

using nlohmann::json;

class SharedData : public EngineObject
{
protected:

	// Keys to avoid when clearing game score
	static const std::vector<s32> GAMESCORE_AVOID_KEYS;

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
	
	// Pause screen
	u8 appPaused;
	std::unique_ptr<Alarm> pauseAlarm;

	// Exit screen
	std::unique_ptr<Alarm> exitTimer;
	bool isLevelPassed;

	// Hourglass
	f32 hourglassRotation = 0.0f;

	// Variables for fade transition
	s8 fadeType;
	std::function<void(void)> fadeCallback;

	// Alarm for time counter
	std::unique_ptr<Alarm> timeAlarm;

	// Font for GUI
	IGUIFont* font;

	// Level score points value for animation purposes
	f32 levelPointsValue;
	f32 globalPointsValue;

	/*
		This map holds all the textures that must be disposed
		at the end of the frame, otherwise severe memory leaks
		will occur, causing memory usage to jump high, leading
		to an unavoidable evitably crash of the application.
	*/
	std::unordered_map<u8, ITexture*> frameResources;

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

	// Jump to next level method
	void jumpToNextLevel();

	// Back to menu room method
	void jumpToMenuRoom();

	// Subtract current level points to global game score
	void subtractLevelPointsOnLose();

	// Reset state on room leave / change
	void resetState();

public:

	// Key for room loader
	static const std::string ROOM_OBJECT_KEY;

	/*
		Shared GUI Render Target Texture.
		All the GUI created by all of the GameObject's subclasses are rendered in this
		render target, which is passed to the shader in the Engine class implementation.
	*/
	ITexture* guiRtt;

	/*
		Global render target.
		In before, this memeber was in the Engine class. Then it has been moved here because
		doing setRenderTarget(0, false, false) didn't work as expected, as the egine binds to
		an undefined RTT, resulting in a black screen.
	*/
	irr::core::array<IRenderTarget> sceneRtts;

	// Constructor
	SharedData();

	// Fade transition alpha value
	f32 fadeValue;

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
	void displayLevelEnd();

	// Display exit menu
	void displayExit();

	// Dispose unneeded resource at frame end
	void disposeResourcesAtFrameEnd();

	/*
		Set function for post processing effect.
		Supply nullptr for callback to erase the desired key.
	*/
	void setPostProcessingCallback(u8 key, std::function<void(const json&)> callback);

	// Trigger post processing callback
	bool triggerPostProcessingCallback(u8 key, const json& data);

	// Stop time
	void stopTime();

	// Invert time
	void invertTime();

	// Check for level timeout
	bool hasLevelTimedOut();

	// Check if application is paused
	bool isAppPaused();
};

#endif // SHAREDDATA_H