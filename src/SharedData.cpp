#include <string>

#include "SharedData.h"
#include "Utility.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "RoomManager.h"

const std::string SharedData::ROOM_OBJECT_KEY = "SharedData";

std::shared_ptr<SharedData> SharedData::singleton = nullptr;

SharedData::SharedData()
{
	// Initialize variables
	gameOverAlpha = 0.0f;
	gameOverSelection = -1;

	fadeType = 0;
	fadeValue = 0.0f;
	fadeCallback = nullptr;

	// Initialize texts for game over
	std::vector<std::wstring> textGameOver;
	textGameOver.push_back(L"Riprova Livello");
	textGameOver.push_back(L"Esci Dal Livello");
	textGroups[KEY_TEXT_GAME_OVER] = textGameOver;

	// Load sounds
	sounds[KEY_SOUND_SELECT] = SoundManager::singleton->getSound(KEY_SOUND_SELECT);
}

void SharedData::update(f32 deltaTime)
{
	// Fade animation
	if (fadeType == 0)
	{
		fadeValue -= 0.002f * deltaTime;

		if (fadeValue < 0.0f)
		{
			fadeValue = 0.0f;

			if (fadeCallback != nullptr)
			{
				fadeCallback();
				fadeCallback = nullptr;
			}
		}
	}
	else if (fadeType == 1)
	{
		fadeValue += 0.002f * deltaTime;

		if (fadeValue > 1.0f)
		{
			fadeValue = 1.0f;

			if (fadeCallback != nullptr)
			{
				fadeCallback();
				fadeCallback = nullptr;
			}
		}
	}

	// Game Over controller
	if (gameOverAlpha > 0)
	{
		// Increment alpha value
		gameOverAlpha += deltaTime * 0.0025f;

		// Clamp value
		if (gameOverAlpha > 1)
		{
			gameOverAlpha = 1;
		}

		// Menu selection
		gameOverRects.clear();

		for (u8 i = 0; i < 2; ++i)
		{
			// Get window size
			const dimension2di windowSize = utility::getWindowSize<s32>(driver);

			// Compute text rectangle
			recti r(0, windowSize.Height / 4 * (2 + i), windowSize.Width, windowSize.Height / 4 * (3 + i));
			gameOverRects.push_back(r);
		}
	}
}

void SharedData::loadAssets()
{
	// Load font
	font = guienv->getFont("fonts/titles.xml");

	// Load textures
	guiTextures[KEY_GUI_COIN] = driver->getTexture("textures/gui_coin.png");
	guiTextures[KEY_GUI_KEY] = driver->getTexture("textures/gui_key.png");
	guiTextures[KEY_GUI_RECTANGLE] = driver->getTexture("textures/gui_rectangle.png");
	guiTextures[KEY_GUI_MOUSE] = driver->getTexture("textures/gui_mouse.png");
}

void SharedData::buildGameScore()
{
	// Draw coin amount
	{
		// Get picked coin amount
		s32 amount = getGameScoreValue(KEY_GUI_COIN);
		if (amount >= 0)
		{
			// Draw coin
			IGUIImage* image = guienv->addImage(guiTextures[KEY_GUI_COIN], vector2di(32, 32));
			image->setMaxSize(dimension2du(128, 128));

			// Draw counter
			IGUIStaticText* text = guienv->addStaticText(std::to_wstring(amount).c_str(), recti(192, 32, 512, 160));
			text->setOverrideFont(font);
			text->setOverrideColor(SColor(255, 255, 255, 255));
			text->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_CENTER);
		}
	}

	// Draw key amount
	{
		// Get picked key amount
		s32 amount = getGameScoreValue(KEY_GUI_KEY);
		if (amount >= 0)
		{
			// Get picked key amount
			s32 pickedAmount = getGameScoreValue(KEY_GUI_KEY_PICKED);

			// Draw keys
			for (u8 i = 0; i < amount; ++i)
			{
				// Compute coords for image
				const s32 x = 32 + 96 * i;
				const s32 y = utility::getWindowSize<s32>(driver).Y - 160;

				// Draw key
				IGUIImage* image = guienv->addImage(guiTextures[KEY_GUI_KEY], vector2di(x, y));
				image->setMaxSize(dimension2du(128, 128));

				// Darken the image only if it hasn't been picked yet
				if (i >= pickedAmount)
				{
					image->setColor(SColor(255, 0, 0, 0));
				}
			}
		}
	}
}

void SharedData::buildGameOver()
{
	// Check if Game Over screen has been triggered
	if (gameOverAlpha > 0)
	{
		// Compute alpha value in unsigned integer form
		u32 alpha[] = {
			(u32)(gameOverAlpha * 192.0f),
			(u32)(gameOverAlpha * 255.0f)
		};

		// Draw background image
		IGUIImage* image = guienv->addImage(guiTextures[KEY_GUI_RECTANGLE], vector2di(0, 0));
		image->setMinSize(utility::getWindowSize<u32>(driver));
		image->setScaleImage(true);
		image->setColor(SColor(alpha[0], 0, 0, 0));

		// Calculate text rectangles, then draw them
		s8 currentSelection = -1;
		const auto& texts = textGroups[KEY_TEXT_GAME_OVER];

		for (u8 i = 0; i < texts.size(); ++i)
		{
			// Default text color
			SColor color(alpha[1], 255, 255, 255);

			// Check if mouse is inside the text rectangle
			if (gameOverRects[i].isPointInside(EventManager::singleton->mousePosition))
			{
				// Make text color different
				color.setBlue(0);

				// Store selection index for later
				currentSelection = i;
			}

			// Draw text
			IGUIStaticText* text = guienv->addStaticText(texts.at(i).c_str(), gameOverRects[i]);
			text->setOverrideFont(font);
			text->setOverrideColor(color);
			text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		}

		// Commit menu selection
		if (gameOverSelection != currentSelection)
		{
			if (currentSelection >= 0)
			{
				playSound(KEY_SOUND_SELECT);
			}
		}
		gameOverSelection = currentSelection;

		// Game Over menu selection
		if (EventManager::singleton->keyStates[KEY_LBUTTON] == KEY_RELEASED)
		{
			if (gameOverSelection == 0)
			{
				SharedData::singleton->startFade(true, std::bind(&SharedData::restartRoom, this));
			}
			else if (gameOverSelection == 1)
			{
				SharedData::singleton->startFade(true, std::bind(&SharedData::jumpToMenuRoom, this));
			}
		}

		// Draw mouse pointer
		ITexture* mouse = guiTextures[KEY_GUI_MOUSE];
		const recti r = utility::getSourceRect(mouse) + EventManager::singleton->mousePosition;
		image = guienv->addImage(r);
		image->setImage(mouse);
	}
}

void SharedData::restartRoom()
{
	// Turn off game over scren
	gameOverAlpha = 0.0f;

	// Restart current room
	RoomManager::singleton->restartRoom();
}

void SharedData::jumpToMenuRoom()
{
	// Turn off game over scren
	gameOverAlpha = 0.0f;

	// Jump to main menu room
	RoomManager::singleton->loadRoom(RoomManager::ROOM_MAIN_MENU);
}

void SharedData::buildFadeTransition()
{
	// Get window size
	recti windowRect(vector2di(0), utility::getWindowSize<s32>(driver));

	// Cover the rectangle for entire window
	IGUIImage* image = guienv->addImage(windowRect);
	image->setColor(SColor((s32)(fadeValue * 255.0f), 255, 255, 255));
	image->setImage(guiTextures[KEY_GUI_RECTANGLE]);
	image->setScaleImage(true);
}

void SharedData::initGameScoreValue(s32 key, s32 value)
{
	gameScores[key] = ScoreValue(1, value);
}

void SharedData::clearGameScore()
{
	gameScores.clear();
}

s32 SharedData::getGameScoreValue(const s32 key, const s32 defaultValue)
{
	// Search for the requested key
	auto search = gameScores.find(key);
	if (search == gameScores.end())
	{
		// Return the default value
		return defaultValue;
	}

	// Return the value for the requested key
	return search->second.value;
}

void SharedData::updateGameScoreValue(const s32 key, const s32 stepValue)
{
	// Search for the requested key
	auto search = gameScores.find(key);
	if (search == gameScores.end())
	{
		// Initialize value
		gameScores[key] = ScoreValue(1, stepValue);
		return;
	}

	// Increment the value by the provided step
	search->second.value += stepValue;
}

void SharedData::startFade(bool in, std::function<void(void)> fadeCallback)
{
	this->fadeType = in ? 1 : 0;
	this->fadeCallback = fadeCallback;
}

void SharedData::buildGUI()
{
	// Game score first
	buildGameScore();

	// Game over screen on top
	buildGameOver();

	// Fade transition over all
	buildFadeTransition();
}

void SharedData::displayGameOver()
{
	gameOverAlpha = 0.05f;
}

void SharedData::setPostProcessingCallback(u8 key, std::function<void(const json&)> callback)
{
	if (callback == nullptr)
	{
		ppCallbacks.erase(ppCallbacks.find(key));
	}
	else
	{
		ppCallbacks[key] = callback;
	}
}

bool SharedData::triggerPostProcessingCallback(u8 key, const json& data)
{
	try
	{
		ppCallbacks.at(key)(data);
		return true;
	}
	catch (std::out_of_range e)
	{
	}
	return false;
}