#include "SharedData.h"

#include <string>

const std::string SharedData::ROOM_OBJECT_KEY = "SharedData";

shared_ptr<SharedData> SharedData::singleton = nullptr;

void SharedData::loadAssets()
{
	// Load font for menu
	font = guienv->getFont("fonts/titles.xml");
}

void SharedData::buildGUIFromGameScore()
{
	// Draw coin amount
	s32 amount = getGameScoreValue(KEY_SCORE_COIN);
	if (amount >= 0)
	{
		ITexture* texture = driver->getTexture("textures/gui_coin.png");
		IGUIImage* image = guienv->addImage(texture, vector2di(32, 32));
		image->setMaxSize(dimension2du(128, 128));

		IGUIStaticText* text = guienv->addStaticText(std::to_wstring(amount).c_str(), recti(192, 32, 512, 128));
		text->setOverrideFont(font);
		text->setOverrideColor(SColor(255, 255, 255, 255));
		text->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_CENTER);
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
	buildGUIFromGameScore();
}