#include <string>

#include "SharedData.h"
#include "Utility.h"

const std::string SharedData::ROOM_OBJECT_KEY = "SharedData";

shared_ptr<SharedData> SharedData::singleton = nullptr;

SharedData::SharedData()
{
	// Initialize variables
	gameOver = 0;
}

void SharedData::stepAnimations(f32 deltaTime)
{
	// Increment Game Over variable
	if (gameOver > 0)
	{
		gameOver += deltaTime * 0.005f;

		// Clamp value
		if (gameOver > 1)
		{
			gameOver = 1;
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
				const s32 y = utility::getWindowSize<s32>(driver).Height - 160;

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
	if (gameOver > 0)
	{
		// Draw background image
		IGUIImage* image = guienv->addImage(guiTextures[KEY_GUI_RECTANGLE], vector2di(0, 0));
		image->setMinSize(utility::getWindowSize<u32>(driver));
		image->setScaleImage(true);
		image->setColor(SColor((u32) (gameOver * 192.0f), 0, 0, 0));
	}
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

void SharedData::buildGUI()
{
	buildGameScore();
	buildGameOver();
}

void SharedData::displayGameOver()
{
	gameOver = 0.05f;
}