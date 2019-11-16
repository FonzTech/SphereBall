#include <string>
#include <iterator>

#include "SharedData.h"
#include "Utility.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "RoomManager.h"
#include "GUIImageSceneNode.h"

const std::string SharedData::ROOM_OBJECT_KEY = "SharedData";

const std::vector<s32> SharedData::GAMESCORE_AVOID_KEYS = {
	KEY_SCORE_POINTS_TOTAL, KEY_SCORE_FRUITS
};

std::shared_ptr<SharedData> SharedData::singleton = nullptr;

SharedData::SharedData()
{
	// Initialize variables
	gameOverAlpha = 0.0f;
	gameOverSelection = -1;

	isLevelPassed = false;

	fadeType = 0;
	fadeValue = 0.0f;
	fadeCallback = nullptr;

	levelPointsValue = 0.0f;
	globalPointsValue = 0.0f;

	guiRtt = nullptr;

	// Initialize texts for game over
	{
		std::vector<std::wstring> textGameOver;
		textGameOver.push_back(L"Retry Level");
		textGameOver.push_back(L"Return To Menu");
		textGroups[KEY_TEXT_GAME_OVER] = textGameOver;
	}

	// Initialize texts for level passed
	{
		std::vector<std::wstring> textLevelPassed;
		textLevelPassed.push_back(L"Next Level");
		textLevelPassed.push_back(L"Return To Menu");
		textGroups[KEY_TEXT_LEVEL_PASSED] = textLevelPassed;
	}

	// Load sounds
	sounds[KEY_SOUND_SELECT] = SoundManager::singleton->getSound(KEY_SOUND_SELECT);
	sounds[KEY_SOUND_CLOCK_A] = SoundManager::singleton->getSound(KEY_SOUND_CLOCK_A);
	sounds[KEY_SOUND_CLOCK_B] = SoundManager::singleton->getSound(KEY_SOUND_CLOCK_B);
	sounds[KEY_SOUND_TIME_OUT] = SoundManager::singleton->getSound(KEY_SOUND_TIME_OUT);

	// Initialize total game score
	initGameScoreValue(KEY_SCORE_POINTS_TOTAL, 0);
}

void SharedData::update(f32 deltaTime)
{
	// Restart room on key press (only for debug builds)
	#if NDEBUG || _DEBUG
	if (EventManager::singleton->keyStates[KEY_KEY_R] == KEY_PRESSED)
	{
		restartRoom();
	}
	#endif

	// Assign delta time
	this->deltaTime = deltaTime;

	// Step alarms
	if (timeAlarm != nullptr)
	{
		timeAlarm->stepDecrement(deltaTime);
		if (timeAlarm->isTriggered())
		{

			// Check for time out
			s32 time = gameScores[KEY_SCORE_TIME].value;
			if (time <= 0)
			{
				// Play time out sound
				playAudio(KEY_SOUND_TIME_OUT);

				// Delete alarm
				timeAlarm = nullptr;
			}
			else
			{
				// Play clock sound
				if (time <= 20)
				{
					f32 volume = (f32)(21 - time) * 5.0f;
					playAudio(time % 2 ? KEY_SOUND_CLOCK_B : KEY_SOUND_CLOCK_A)->setVolume(volume);
				}

				// Reset alarm
				timeAlarm->setTime(1000.0f);

				// Decrease time
				updateGameScoreValue(KEY_SCORE_TIME, -1);
			}
		}
	}

	// Check for exit timer
	if (exitTimer != nullptr)
	{
		exitTimer->stepDecrement(deltaTime);
		if (exitTimer->isTriggered())
		{
			// Display exit screen
			displayLevelEnd();

			// Delete timer
			exitTimer = nullptr;
		}
	}

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

		// Get window size
		const vector2di windowSize = Utility::getWindowSize<s32>(driver);

		for (u8 i = 0; i < 2; ++i)
		{

			// Compute text rectangle
			s32 y = (s32)(windowSize.Y * 0.6f + 128.0f * (f32)i);
			recti r(0, y, windowSize.X, y + 128);
			gameOverRects.push_back(r);
		}
	}

	// Animate hourglass angle
	hourglassRotation -= deltaTime * 0.0025f;
	if (hourglassRotation < 0.0f)
	{
		hourglassRotation = 0.0f;
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
	guiTextures[KEY_GUI_HOURGLASS] = driver->getTexture("textures/gui_hourglass.png");
	guiTextures[KEY_GUI_HOURGLASS_SAND_TOP] = driver->getTexture("textures/gui_hourglass_sand_top.png");
	guiTextures[KEY_GUI_HOURGLASS_SAND_BOTTOM] = driver->getTexture("textures/gui_hourglass_sand_bottom.png");
	guiTextures[KEY_GUI_APPLE] = driver->getTexture("textures/gui_apple.png");
	guiTextures[KEY_GUI_BANANA] = driver->getTexture("textures/gui_banana.png");
	guiTextures[KEY_GUI_STRAWBERRY] = driver->getTexture("textures/gui_strawberry.png");
	guiTextures[KEY_GUI_WATERMELON] = driver->getTexture("textures/gui_watermelon.png");
	guiTextures[KEY_GUI_PINEAPPLE] = driver->getTexture("textures/gui_pineapple.png");
}

void SharedData::buildGameScore()
{
	// Get window size
	const vector2di windowSize = Utility::getWindowSize<s32>(driver);

	// Get alpha value for Level HUD
	const s32 alpha = (s32)(255.0f - gameOverAlpha * 255.0f);

	// Draw coin amount
	{
		// Get picked coin amount
		s32 amount = getGameScoreValue(KEY_SCORE_COIN);
		if (amount >= 0)
		{
			// Common color
			const SColor color(alpha, 255, 255, 255);

			// Draw coin
			IGUIImage* image = guienv->addImage(guiTextures[KEY_GUI_COIN], vector2di(32, 32));
			image->setMaxSize(dimension2du(128, 128));
			image->setColor(color);

			// Draw counter
			IGUIStaticText* text = guienv->addStaticText(std::to_wstring(amount).c_str(), recti(192, 32, 512, 160));
			text->setOverrideFont(font);
			text->setOverrideColor(color);
			text->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_CENTER);
		}
	}

	// Draw key amount
	{
		// Get picked key amount
		s32 amount = getGameScoreValue(KEY_SCORE_KEY_TOTAL);
		if (amount >= 0)
		{
			// Get picked key amount
			s32 pickedAmount = getGameScoreValue(KEY_SCORE_KEY_PICKED);

			// Draw keys
			for (u8 i = 0; i < amount; ++i)
			{
				// Compute coords for image
				const s32 x = 32 + 96 * i;
				const s32 y = windowSize.Y - 160;

				// Draw key
				IGUIImage* image = guienv->addImage(guiTextures[KEY_GUI_KEY], vector2di(x, y));
				image->setMaxSize(dimension2du(128, 128));

				// Darken the image only if it hasn't been picked yet
				const u32 cc = i >= pickedAmount ? 0 : 255;
				image->setColor(SColor(alpha, cc, cc, cc));
			}
		}
	}

	// Draw time
	{
		s32 amount = getGameScoreValue(KEY_SCORE_TIME);
		if (amount >= 0)
		{
			// Get maximum time
			s32 maxTime = getGameScoreValue(KEY_SCORE_TIME_MAX);

			// Get time ratio
			f32 ratio = (f32)amount / (f32)maxTime;

			// Compute coords for image
			dimension2du imageSize = guiTextures[KEY_GUI_HOURGLASS]->getOriginalSize();
			dimension2du size(192 * imageSize.Width / imageSize.Height, 192);

			vector2df hudSize;

			// Render sand on separate texture
			frameResources[KEY_GUI_HOURGLASS] = driver->addRenderTargetTexture(dimension2d<u32>(128, 256));
			{
				driver->setRenderTarget(frameResources[KEY_GUI_HOURGLASS]);

				dimension2du size = frameResources[KEY_GUI_HOURGLASS]->getSize();

				// Data for later
				recti destRect;

				// Top part
				{
					// Compute source rect
					const recti sourceRect = Utility::getSourceRect(guiTextures[KEY_GUI_HOURGLASS_SAND_TOP]);

					// Compute common width
					hudSize.Y = 192.0f;
					hudSize.X = (f32)sourceRect.getWidth() / (f32)sourceRect.getHeight() * hudSize.Y;

					// Compute common destination rect
					destRect = recti(vector2di(0), size);

					// Compute clip rect
					f32 height = (f32)destRect.getHeight() * 0.5f;
					const recti clipRect(0, (s32)(height * (1.0f - ratio)), destRect.getWidth(), destRect.getHeight());

					// Draw rectangle
					driver->draw2DImage(guiTextures[KEY_GUI_HOURGLASS_SAND_TOP], destRect, sourceRect, &clipRect, 0, true);
				}

				// Bottom part
				{
					// Compute source rect
					const recti sourceRect = Utility::getSourceRect(guiTextures[KEY_GUI_HOURGLASS_SAND_BOTTOM]);

					// Compute common destination rect
					f32 height = (f32)destRect.getHeight() * 0.5f;
					const recti clipRect(0, (s32)height + (s32)(height * ratio), destRect.getWidth(), destRect.getHeight());

					// Draw rectangle
					driver->draw2DImage(guiTextures[KEY_GUI_HOURGLASS_SAND_BOTTOM], destRect, sourceRect, &clipRect, 0, true);
				}

				// Front part
				{
					const recti sourceRect = Utility::getSourceRect(guiTextures[KEY_GUI_HOURGLASS]);
					driver->draw2DImage(guiTextures[KEY_GUI_HOURGLASS], destRect, sourceRect, 0, 0, true);
				}

				// Restore old render target
				driver->setRenderTarget(0, false, false);
			}

			// Render hourglass on separate texture
			frameResources[KEY_GUI_HOURGLASS_SAND_TOP] = driver->addRenderTargetTexture(dimension2d<u32>(256, 256));
			{
				driver->setRenderTarget(frameResources[KEY_GUI_HOURGLASS_SAND_TOP]);

				// Get container size
				size = (dimension2df)frameResources[KEY_GUI_HOURGLASS_SAND_TOP]->getSize();

				const vector3df containerSize((f32)size.Width, (f32)size.Height, 1.0f);
				const vector3df halfSize = containerSize * 0.5f - vector3df(hudSize.X, hudSize.Y, 0.0f) * 0.5f;

				vector3df vertices[] =
				{
					vector3df(0) + halfSize,
					vector3df(hudSize.X, 0, 0) + halfSize,
					vector3df(0, hudSize.Y, 0) + halfSize,
					vector3df(hudSize.X, hudSize.Y, 0) + halfSize
				};

				// Rotation animation
				f32 rotationValue;
				{
					rotationValue = Utility::getCubicBezierAt(vector2df(0.25f, 0.1f), vector2df(0.25f, 1.0f), hourglassRotation).Y;
					rotationValue = degToRad(rotationValue * 180.0f);
				}

				// Draw hourglass on a separate quad
				GUIImageSceneNode imageNode(smgr->getRootSceneNode(), smgr, -1);
				imageNode.setVertices(containerSize, vertices[0], vertices[1], vertices[2], vertices[3], &vector3df(containerSize.X * 0.5f, containerSize.Y * 0.5f, 0.0f), &vector3df(0, 0, rotationValue));
				imageNode.getMaterial(0).setTexture(0, frameResources[KEY_GUI_HOURGLASS]);
				imageNode.getMaterial(0).setFlag(EMF_BLEND_OPERATION, true);
				imageNode.render();
				imageNode.remove();

				// Restore old render target
				driver->setRenderTarget(0, false, false);
			}

			// Compute final position
			const vector2di position(windowSize.X - size.Width, windowSize.Y - size.Height);

			// Draw full hourglass
			IGUIImage* image = guienv->addImage(frameResources[KEY_GUI_HOURGLASS_SAND_TOP], position);
			image->setColor(SColor(alpha, 255, 255, 255));
			
			// Draw remaining time
			if (amount <= 20)
			{
				f32 coeff = 1.0f - (f32)amount / 20.0f;
				u32 timeAlpha = std::min(std::max((s32)((coeff * 510.0f) * (1.0f - gameOverAlpha)), 0), 255);

				IGUIStaticText* text = guienv->addStaticText(std::to_wstring(amount).c_str(), recti(position, size));
				text->setOverrideFont(font);
				text->setOverrideColor(SColor(timeAlpha, 255, 255, 255));
				text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
			}
		}
	}

	// Animate score points value
	{
		f32 points = (f32)getGameScoreValue(KEY_SCORE_POINTS);
		Utility::animateFloatValue(deltaTime, &levelPointsValue, points);
	}

	// Draw score points
	for (int i = 0; i < 2; ++i)
	{
		// Get value to display
		s32 amount = i ? (s32)levelPointsValue : getGameScoreValue(KEY_SCORE_POINTS_TOTAL);

		// Compute vertical position
		s32 y = i ? 192 : 112;
		SColor color = i ? SColor(alpha, 255, 255, 255) : SColor(alpha, 192, 192, 192);

		// Draw counter
		IGUIStaticText* text = guienv->addStaticText(std::to_wstring(amount).c_str(), recti(windowSize.X / 8 * 3, windowSize.Y - y, windowSize.X, windowSize.Y - y + 96));
		text->setOverrideFont(font);
		text->setOverrideColor(color);
		text->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_CENTER);
	}

	// Draw fruits
	{
		u8 fruitKeys[] = {
			KEY_GUI_APPLE, KEY_GUI_BANANA, KEY_GUI_STRAWBERRY, KEY_GUI_WATERMELON, KEY_GUI_PINEAPPLE
		};

		for (int i = 0; i < 5; ++i)
		{
			s32 x = 640 - i * 128;
			s32 color = getGameScoreValue(KEY_SCORE_FRUITS) > i ? 255 : 0;

			IGUIImage* image = guienv->addImage(guiTextures[fruitKeys[i]], vector2di(windowSize.X - x, 8));
			image->setColor(SColor(alpha, color, color, color));
		}
	}
}

void SharedData::buildGameOver()
{
	// Check if Game Over screen has been triggered
	if (gameOverAlpha <= 0)
	{
		return;
	}

	// Animate global points value
	if (gameOverAlpha >= 1.0f)
	{
		f32 points = (f32)(getGameScoreValue(KEY_SCORE_POINTS_TOTAL) + getGameScoreValue(KEY_SCORE_POINTS) * (isLevelPassed ? 1 : -1));
		Utility::animateFloatValue(deltaTime, &globalPointsValue, points);
	}

	// Get window size
	vector2df windowSize = Utility::getWindowSize<f32>(driver);

	// Compute alpha value in unsigned integer form
	u32 alpha[] = {
		(u32)(gameOverAlpha * 192.0f),
		(u32)(gameOverAlpha * 255.0f)
	};

	// Draw background image
	IGUIImage* image = guienv->addImage(guiTextures[KEY_GUI_RECTANGLE], vector2di(0, 0));
	image->setMinSize(Utility::getWindowSize<u32>(driver));
	image->setScaleImage(true);
	image->setColor(SColor(alpha[0], 0, 0, 0));

	// Calculate text rectangles, then draw them
	s8 currentSelection = -1;
	const auto& texts = textGroups[isLevelPassed ? KEY_TEXT_LEVEL_PASSED : KEY_TEXT_GAME_OVER];

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
			playAudio(KEY_SOUND_SELECT);
		}
	}
	gameOverSelection = currentSelection;

	// Draw completion percentage
	for (int i = 0; i < 3; ++i)
	{
		s32 y = (s32)(windowSize.Y * 0.1f) + 128 * i;

		std::wstring str;
		SColor color;

		if (i == 0)
		{
			const std::wstring prefix = isLevelPassed ? L"Level Passed: " : L"Level Failed: ";
			color = isLevelPassed ? SColor(alpha[1], 0, 255, 0) : SColor(alpha[1], 255, 64, 64);

			const f32 percentage = std::floor((f32)gameScores[KEY_SCORE_ITEMS_PICKED].value / (f32)gameScores[KEY_SCORE_ITEMS_MAX].value * 100.0f);
			str = prefix + std::to_wstring((s32)percentage) + L"%";
		}
		else if (i == 1)
		{
			str = L"Level Points: " + std::to_wstring(getGameScoreValue(KEY_SCORE_POINTS));
			color = SColor(255, 255, 255, 0);
		}
		else if (i == 2)
		{
			str = L"Total Points: " + std::to_wstring((s32)globalPointsValue);
			color = SColor(255, 192, 192, 192);
		}

		IGUIStaticText* text = guienv->addStaticText(str.c_str(), recti(0, y, (s32)windowSize.X, y + 96));
		text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
		text->setOverrideColor(color);
		text->setOverrideFont(font);
	}

	// Game Over menu selection
	if (EventManager::singleton->keyStates[KEY_LBUTTON] == KEY_RELEASED)
	{
		if (gameOverSelection == 0)
		{
			const auto functionToTrigger = isLevelPassed ? &SharedData::jumpToNextLevel : &SharedData::restartRoom;
			startFade(true, std::bind(functionToTrigger, this));
		}
		else if (gameOverSelection == 1)
		{
			startFade(true, std::bind(&SharedData::jumpToMenuRoom, this));
		}
	}

	// Draw mouse pointer
	ITexture* mouse = guiTextures[KEY_GUI_MOUSE];
	const recti r = Utility::getSourceRect(mouse) + EventManager::singleton->mousePosition;
	image = guienv->addImage(r);
	image->setImage(mouse);
}

void SharedData::jumpToNextLevel()
{
	// Add current level score to global score
	s32 points = getGameScoreValue(KEY_SCORE_POINTS);
	updateGameScoreValue(KEY_SCORE_POINTS_TOTAL, points);

	// Reset state
	resetState();

	// Restart current room
	RoomManager::singleton->jumpToNextLevel();
}

void SharedData::restartRoom()
{
	// Update game score
	subtractLevelPointsOnLose();

	// Reset state
	resetState();

	// Restart current room
	RoomManager::singleton->restartRoom();
}

void SharedData::jumpToMenuRoom()
{
	// Update game score
	subtractLevelPointsOnLose();

	// Turn off game over scren
	resetState();

	// Jump to main menu room
	RoomManager::singleton->loadRoom(RoomManager::ROOM_MAIN_MENU);
}

void SharedData::subtractLevelPointsOnLose()
{
	if (gameOverAlpha > 0.0f && !isLevelPassed)
	{
		updateGameScoreValue(KEY_SCORE_POINTS_TOTAL, -getGameScoreValue(KEY_SCORE_POINTS, 0));
	}
}

void SharedData::resetState()
{
	// Turn off game over scren
	gameOverAlpha = 0.0f;

	// Level not passed by default
	isLevelPassed = false;

	// Reset post-processing effects
	json data = {
		{ "speed", 0.0f },
		{ "strength", 0.0f }
	};
	triggerPostProcessingCallback(KEY_PP_WAVE, data);
}

void SharedData::buildFadeTransition()
{
	// Get window size
	recti windowRect(vector2di(0), Utility::getWindowSize<s32>(driver));

	// Cover the rectangle for entire window
	IGUIImage* image = guienv->addImage(windowRect);
	image->setColor(SColor((s32)(fadeValue * 255.0f), 255, 255, 255));
	image->setImage(guiTextures[KEY_GUI_RECTANGLE]);
	image->setScaleImage(true);
}

void SharedData::initGameScoreValue(s32 key, s32 value)
{
	// Init game score
	gameScores[key] = ScoreValue(1, value);

	// Check for special key
	if (key == KEY_SCORE_TIME)
	{
		timeAlarm = std::make_unique<Alarm>(1000.0f);
	}
}

void SharedData::clearGameScore()
{
	// Clear game score map
	for (auto it = gameScores.cbegin(); it != gameScores.cend();)
	{
		if (std::find(GAMESCORE_AVOID_KEYS.cbegin(), GAMESCORE_AVOID_KEYS.cend(), it->first) == GAMESCORE_AVOID_KEYS.cend())
		{
			it = gameScores.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Remove alarm for time counter
	timeAlarm = nullptr;

	// Reset level score value
	levelPointsValue = 0.0f;
}

s32 SharedData::getGameScoreValue(const s32 key, const s32 defaultValue)
{
	// Search for the requested key
	const auto& search = gameScores.find(key);
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
	const auto& search = gameScores.find(key);
	if (search == gameScores.end())
	{
		// Initialize value
		gameScores[key] = ScoreValue(1, stepValue);
		return;
	}

	// Increment the value by the provided step
	search->second.value += stepValue;
}

void SharedData::startFade(bool in, std::function<void(void)> fadeCallback, f32 value)
{
	this->fadeType = in ? 1 : 0;
	this->fadeValue = value;
	this->fadeCallback = fadeCallback;
}

void SharedData::startFade(bool in, std::function<void(void)> fadeCallback)
{
	startFade(in, fadeCallback, fadeValue);
}

void SharedData::buildGUI()
{
	// Draw in-level HUD
	if (RoomManager::singleton->isCurrentRoomALevel())
	{
		// Game score first
		buildGameScore();

		// Game over screen on top
		buildGameOver();
	}

	// Fade transition over all
	buildFadeTransition();
}

void SharedData::displayLevelEnd()
{
	// Trigger game over for GUI
	gameOverAlpha = 0.05f;

	// Remove time counter alarm
	timeAlarm = nullptr;
}

void SharedData::displayExit()
{
	// Stop time elapsing
	stopTime();

	// Mark level as passed
	isLevelPassed = true;

	// Trigger exit menu
	exitTimer = std::make_unique<Alarm>(750.0f);
}

void SharedData::disposeResourcesAtFrameEnd()
{
	for (auto& entry : frameResources)
	{
		driver->removeTexture(entry.second);
	}
	frameResources.clear();
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

void SharedData::stopTime()
{
	// Remove time counter alarm
	timeAlarm = nullptr;
}

void SharedData::invertTime()
{
	// Invert time
	s32 nowTime = getGameScoreValue(KEY_SCORE_TIME, 0);
	s32 maxTime = getGameScoreValue(KEY_SCORE_TIME_MAX, 0);
	initGameScoreValue(KEY_SCORE_TIME, maxTime - nowTime);

	// Setup animation
	hourglassRotation = 1;

	// Trigger ripple effectx
	const vector2df windowSize = Utility::getWindowSize<f32>(driver);
	const vector2df position(windowSize.X - 128, windowSize.Y - 128);

	json data = {
		{ "x", position.X / windowSize.X },
		{ "y", position.Y / windowSize.Y },
		{ "z", 1.0f }
	};
	triggerPostProcessingCallback(KEY_PP_RIPPLE, data);
}

bool SharedData::hasLevelTimedOut()
{
	return getGameScoreValue(KEY_SCORE_TIME) <= 0 && timeAlarm == nullptr;
}