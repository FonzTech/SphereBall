#include <string>

#include "SharedData.h"
#include "Utility.h"
#include "EventManager.h"
#include "SoundManager.h"

const std::string SharedData::ROOM_OBJECT_KEY = "SharedData";

shared_ptr<SharedData> SharedData::singleton = nullptr;

SharedData::SharedData()
{
	// Initialize variables
	selection = -1;
	gameOver = 0;

	// Initialize texts for game over
	vector<wstring> textGameOver;
	textGameOver.push_back(L"Riprova Livello");
	textGameOver.push_back(L"Esci Dal Livello");
	textGroups[KEY_TEXT_GAME_OVER] = textGameOver;

	// Load sounds
	sounds[KEY_SOUND_SELECT] = SoundManager::singleton->getSound(KEY_SOUND_SELECT);
}

void SharedData::stepAnimations(f32 deltaTime)
{
	// Increment Game Over variable
	if (gameOver > 0)
	{
		gameOver += deltaTime * 0.0025f;

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
		// Compute alpha value in unsigned integer form
		u32 alpha[] = {
			(u32)(gameOver * 192.0f),
			(u32)(gameOver * 255.0f)
		};

		// Draw background image
		IGUIImage* image = guienv->addImage(guiTextures[KEY_GUI_RECTANGLE], vector2di(0, 0));
		image->setMinSize(utility::getWindowSize<u32>(driver));
		image->setScaleImage(true);
		image->setColor(SColor(alpha[0], 0, 0, 0));

		// Get window size
		const dimension2di windowSize = utility::getWindowSize<s32>(driver);

		// Calculate text rectangles, then draw them
		s8 currentSelection = -1;
		const auto& texts = textGroups[KEY_TEXT_GAME_OVER];

		for (u8 i = 0; i < texts.size(); ++i)
		{
			// Compute text rectangle
			const recti r(0, windowSize.Height / 4 * (2 + i), windowSize.Width, windowSize.Height / 4 * (3 + i));

			// Default text color
			SColor color(alpha[1], 255, 255, 255);

			// Check if mouse is inside the text rectangle
			if (r.isPointInside(EventManager::singleton->mousePosition))
			{
				// Make text color different
				color.setBlue(0);

				// Store selection index for later
				currentSelection = i;
			}

			// Draw text
			IGUIStaticText* text = guienv->addStaticText(texts.at(i).c_str(), r);
			text->setOverrideFont(font);
			text->setOverrideColor(color);
			text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		}

		// Commit menu selection
		if (selection != currentSelection)
		{
			if (currentSelection >= 0)
			{
				playSound(KEY_SOUND_SELECT);
			}
		}
		selection = currentSelection;

		// Draw mouse pointer
		ITexture* mouse = guiTextures[KEY_GUI_MOUSE];
		const recti r = utility::getSourceRect(mouse) + EventManager::singleton->mousePosition;
		image = guienv->addImage(r);
		image->setImage(mouse);
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