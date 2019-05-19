#include <string>

#include "SharedData.h"
#include "Utility.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "RoomManager.h"
#include "GUIImageSceneNode.h"

const std::string SharedData::ROOM_OBJECT_KEY = "SharedData";

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

	// Initialize texts for game over
	{
		std::vector<std::wstring> textGameOver;
		textGameOver.push_back(L"Riprova Livello");
		textGameOver.push_back(L"Esci Dal Livello");
		textGroups[KEY_TEXT_GAME_OVER] = textGameOver;
	}

	// Initialize texts for level passed
	{
		std::vector<std::wstring> textLevelPassed;
		textLevelPassed.push_back(L"Livello Successivo");
		textLevelPassed.push_back(L"Torna Al Menu");
		textGroups[KEY_TEXT_LEVEL_PASSED] = textLevelPassed;
	}

	// Load sounds
	sounds[KEY_SOUND_SELECT] = SoundManager::singleton->getSound(KEY_SOUND_SELECT);
	sounds[KEY_SOUND_CLOCK_A] = SoundManager::singleton->getSound(KEY_SOUND_CLOCK_A);
	sounds[KEY_SOUND_CLOCK_B] = SoundManager::singleton->getSound(KEY_SOUND_CLOCK_B);
	sounds[KEY_SOUND_TIME_OUT] = SoundManager::singleton->getSound(KEY_SOUND_TIME_OUT);
}

void SharedData::update(f32 deltaTime)
{
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
				playSound(KEY_SOUND_TIME_OUT);

				// Delete alarm
				timeAlarm = nullptr;
			}
			else
			{
				// Decrease time
				updateGameScoreValue(KEY_SCORE_TIME, -1);

				// Play clock sound
				if (time <= 20)
				{
					f32 volume = (f32)(21 - time) * 5.0f;
					playSound(time % 2 ? KEY_SOUND_CLOCK_B : KEY_SOUND_CLOCK_A)->setVolume(volume);
				}

				// Reset alarm
				timeAlarm->setTime(1000.0f);
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

		for (u8 i = 0; i < 2; ++i)
		{
			// Get window size
			const dimension2di windowSize = utility::getWindowSize<s32>(driver);

			// Compute text rectangle
			recti r(0, windowSize.Height / 4 * (2 + i), windowSize.Width, windowSize.Height / 4 * (3 + i));
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
	guiTextures[KEY_GUI_HOURGLASS_GLOW] = driver->getTexture("textures/gui_hourglass_glow.png");
}

void SharedData::buildGameScore()
{
	vector2di windowSize = utility::getWindowSize<s32>(driver);

	// Draw coin amount
	{
		// Get picked coin amount
		s32 amount = getGameScoreValue(KEY_SCORE_COIN);
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
				if (i >= pickedAmount)
				{
					image->setColor(SColor(255, 0, 0, 0));
				}
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
					const recti sourceRect = utility::getSourceRect(guiTextures[KEY_GUI_HOURGLASS_SAND_TOP]);

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
					const recti sourceRect = utility::getSourceRect(guiTextures[KEY_GUI_HOURGLASS_SAND_BOTTOM]);

					// Compute common destination rect
					f32 height = (f32)destRect.getHeight() * 0.5f;
					const recti clipRect(0, (s32)height + (s32)(height * ratio), destRect.getWidth(), destRect.getHeight());

					// Draw rectangle
					driver->draw2DImage(guiTextures[KEY_GUI_HOURGLASS_SAND_BOTTOM], destRect, sourceRect, &clipRect, 0, true);
				}

				// Front part
				{
					const recti sourceRect = utility::getSourceRect(guiTextures[KEY_GUI_HOURGLASS]);
					driver->draw2DImage(guiTextures[KEY_GUI_HOURGLASS], destRect, sourceRect, 0, 0, true);
				}

				// Draw glow
				{
					const recti sourceRect = utility::getSourceRect(guiTextures[KEY_GUI_HOURGLASS_GLOW]);
					destRect = recti(0, size.Height / 2 - size.Width / 2, size.Width, size.Height / 2 + size.Width / 2);

					f32 hourglassAlpha;
					if (hourglassRotation < 0.5f)
					{
						hourglassAlpha = utility::getCubicBezierAt(vector2df(0.0f, 1.0f), vector2df(0.0f, 1.0f), hourglassRotation * 2.0f).Y;
					}
					else
					{
						hourglassAlpha = 1;
					}

					const u32 alpha = (u32)(hourglassAlpha * 255.0f);
					const SColor color(alpha, 255, 255, 255);
					const SColor colors[] = { color , color , color, color  };

					driver->draw2DImage(guiTextures[KEY_GUI_HOURGLASS_GLOW], destRect, sourceRect, 0, colors, true);
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
					rotationValue = utility::getCubicBezierAt(vector2df(0.25f, 0.1f), vector2df(0.25f, 1.0f), hourglassRotation).Y;
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
			guienv->addImage(frameResources[KEY_GUI_HOURGLASS_SAND_TOP], position);

			// Draw remaining time
			if (amount <= 20)
			{
				f32 coeff = 1.0f - (f32)amount / 20.0f;
				s32 alpha = std::min((s32)(coeff * 510.0f), 255);

				IGUIStaticText* text = guienv->addStaticText(std::to_wstring(amount).c_str(), recti(position, size));
				text->setOverrideFont(font);
				text->setOverrideColor(SColor(alpha, 255, 255, 255));
				text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
			}
		}
	}


	// Draw score points
	for (int i = 0; i < 2; ++i)
	{
		// Get value to display
		s32 amount = getGameScoreValue(i ? KEY_SCORE_POINTS : KEY_SCORE_POINTS_TOTAL);

		// Compute vertical position
		s32 y = i ? 192 : 112;
		SColor color = i ? SColor(255, 255, 255, 255) : SColor(255, 192, 192, 192);

		// Draw counter
		IGUIStaticText* text = guienv->addStaticText(std::to_wstring(amount).c_str(), recti(windowSize.X / 8 * 3, windowSize.Y - y, windowSize.X, windowSize.Y - y + 96));
		text->setOverrideFont(font);
		text->setOverrideColor(color);
		text->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_CENTER);
	}
}

void SharedData::buildGameOver()
{
	// Check if Game Over screen has been triggered
	if (gameOverAlpha > 0)
	{
		// Get window size
		vector2df windowSize = utility::getWindowSize<f32>(driver);

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
				playSound(KEY_SOUND_SELECT);
			}
		}
		gameOverSelection = currentSelection;

		// Draw completion percentage
		{
			const std::wstring prefix = std::wstring(isLevelPassed ? L"Level Passed: " : L"Level Failed: ");
			const SColor color = isLevelPassed ? SColor(alpha[1], 0, 255, 0) : SColor(alpha[1], 255, 64, 64);

			const f32 percentage = std::floor((f32)gameScores[KEY_SCORE_ITEMS_PICKED].value / (f32)gameScores[KEY_SCORE_ITEMS_MAX].value * 100.0f);
			const std::wstring str = prefix + std::to_wstring((s32)percentage) + std::wstring(L"%");

			IGUIStaticText* text = guienv->addStaticText(str.c_str(), recti(0, 0, (s32)windowSize.X, (s32)(windowSize.Y * 0.5f)));
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
				SharedData::singleton->startFade(true, std::bind(functionToTrigger, this));
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

void SharedData::jumpToNextLevel()
{
	// Turn off game over scren
	gameOverAlpha = 0.0f;

	// Restart current room
	RoomManager::singleton->jumpToNextLevel();
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
	gameScores.clear();

	// Remove alarm for time counter
	timeAlarm = nullptr;
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
	// Game score first
	buildGameScore();

	// Game over screen on top
	buildGameOver();

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
}