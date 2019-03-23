#include "MainMenu.h"
#include "RoomManager.h"
#include "SoundManager.h"
#include "Camera.h"

shared_ptr<MainMenu> MainMenu::createInstance(const json &jsonData)
{
	return make_shared<MainMenu>();
}

MainMenu::MainMenu() : GameObject()
{
	// Load font for menu
	font = guienv->getFont("fonts/titles.xml");

	// Load mouse pointer texture
	mouse = driver->getTexture("textures/gui_mouse.png");
	rectangleTexture = driver->getTexture("textures/gui_rectangle.png");

	// Create texts for main menu
	optionTitles.push_back(L"Start Game");
	optionTitles.push_back(L"Level Editor");
	optionTitles.push_back(L"Options");
	optionTitles.push_back(L"Exit");

	// Create texts for options
	optionTitles.push_back(L"Resolution");
	optionTitles.push_back(L"SFX Volume");
	optionTitles.push_back(L"Music Volume");
	optionTitles.push_back(L"Back");

	// Create texts for levels
	levelTitles.push_back(L"Choose The Level To Play");
	levelTitles.push_back(L"Go Back");

	for (u8 i = 1; i <= 10; ++i)
	{
		levelTitles.push_back(std::to_wstring(i));
	}

	// Load sound
	sounds[KEY_SOUND_SELECT] = SoundManager::singleton->getSound(KEY_SOUND_SELECT);
	sounds[KEY_SOUND_SELECT]->setRelativeToListener(true);

	// Initialize members
	currentSection = 1;
	currentIndex = -1;
	animation = 0;
}

void MainMenu::update()
{
	// Get window size
	windowSize = utility::getWindowSize<s32>(driver);

	// Temporary current index
	s8 tmpIndex = -1;

	// Clear previous computed areas
	optionsAreas.clear();
	levelAreas.clear();

	// Compute vertical sections for menu
	for (s8 i = 0; i < 8; ++i)
	{
		// Calculate fixed horizonal sections for entries
		s32 x1 = (s32)((f32)windowSize.Width * 0.25f) + (i >= 4 ? windowSize.Width : 0);
		s32 x2 = (s32)((f32)windowSize.Width * 0.75f) + (i >= 4 ? windowSize.Width : 0);

		// Calculate vertical position for entries
		s32 y1 = (s32)(windowSize.Height * (0.3f + 0.1f * (i % 4)));
		s32 y2 = (s32)(windowSize.Height * (0.4f + 0.1f * (i % 4)));
		optionsAreas.push_back(recti(x1, y1, x2, y2));

		// Check mouse in area
		if (currentSection != 0 && optionsAreas[i].isPointInside(EventManager::singleton->mousePosition))
		{
			tmpIndex = i;
		}
	}

	// Compute sections for levels
	{
		// Common top limit
		const s32 height = max(windowSize.Height / 6, 96);

		// Description
		levelAreas.push_back(recti(0, 0, (s32)(windowSize.Width * 0.6f), height));

		// Back button
		levelAreas.push_back(recti((s32)(windowSize.Width * 0.5f), 0, (s32)(windowSize.Width * 0.95f), height));

		// Levels
		for (s32 i = 0; i < 2; ++i)
		{
			for (s32 j = 0; j < 5; ++j)
			{
				const s32 row = (s32)(floor((f32)i / 5.0f));

				const s32 x1 = windowSize.Width / 5 * j;
				const s32 x2 = windowSize.Width / 5 * (j + 1);

				const s32 y1 = windowSize.Height / 2 + (height * (i - 1));
				const s32 y2 = windowSize.Height / 2 + (height * i);

				levelAreas.push_back(recti(x1, y1, x2, y2));
			}
		}

		// Check mouse in area
		if (currentSection == 0)
		{
			for (u8 i = 1; i != levelAreas.size(); ++i)
			{
				if (levelAreas[i].isPointInside(EventManager::singleton->mousePosition))
				{
					tmpIndex = i;
				}
			}
		}
	}

	// Change current index and trigger sound
	if (currentIndex != tmpIndex)
	{
		currentIndex = tmpIndex;
		if (currentIndex >= 0)
		{
			playSound(KEY_SOUND_SELECT);
		}
	}

	// Check for left mouse click
	if (currentIndex >= 0 && EventManager::singleton->keyStates[KEY_LBUTTON] == KEY_RELEASED)
	{
		if (currentSection == 0)
		{
			if (currentIndex == 1)
			{
				currentSection = 1;
			}
			else
			{
				std::string fname = "level_" + std::to_string(currentIndex - 1);
				RoomManager::singleton->loadRoom(fname);
			}
		}
		else
		{
			if (currentIndex == 0)
			{
				if (currentSection == 1)
				{
					currentSection = 0;
				}
			}
			else if (currentIndex == 2)
			{
				if (currentSection == 1)
				{
					currentSection = 2;
				}
			}
			else if (currentIndex == 3)
			{
				if (currentSection == 1)
				{
					RoomManager::singleton->isProgramRunning = false;
				}
				else
				{
					currentSection = 1;
				}
			}
		}
	}

	// Animate sections
	if (currentSection == 0)
	{
		animation -= 0.0025f * deltaTime;
		if (animation <= -1)
		{
			animation = -1;
		}
	}
	else if (currentSection == 1)
	{
		if (animation > 0)
		{
			animation -= 0.0025f * deltaTime;
		}
		else if (animation < 0)
		{
			animation += 0.0025f * deltaTime;
		}

		if (animation >= -0.05 && animation <= 0.05)
		{
			animation = 0;
		}
	}
	else if (currentSection == 2)
	{
		animation += 0.0025f * deltaTime;
		if (animation >= 1)
		{
			animation = 1;
		}
	}
}

void MainMenu::draw()
{
	// Reposition camera
	Camera::singleton->position = position + vector3df(0, 40, -100);
	Camera::singleton->lookAt = position;

	// Get animated value
	f32 animatedValue = utility::getCubicBezierAt(*(&vector2df(0, 1)), *(&vector2df(1, 0)), abs(animation)).X;

	// Draw main menu and options
	size_t limit = animation < 0 ? optionsAreas.size() / 2 : optionsAreas.size();
	for (u8 i = 0; i != limit; ++i)
	{
		// Get right color
		SColor color = i % 4 == currentIndex ? SColor(255, 255, 255, 0) : SColor(255, 255, 255, 255);

		// Compute right animated position
		s32 x = (s32)(animatedValue * (f32)windowSize.Width * (animation < 0 ? 1.0f : -1.0f));
		optionsAreas[i].UpperLeftCorner.X += x;
		optionsAreas[i].LowerRightCorner.X += x;

		// Create text to be drawn
		IGUIStaticText* text = guienv->addStaticText(optionTitles[i].c_str(), optionsAreas[i]);
		text->setOverrideFont(font);
		text->setOverrideColor(color);
		text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
	}

	// Draw level entries
	if (animation < 0)
	{
		for (u8 i = 0; i != levelAreas.size(); ++i)
		{
			// Compute final animated coordinate offset
			s32 x = (s32)((1 - animatedValue) * (f32)windowSize.Width * -1.0f);

			if (i >= 2)
			{
				// Background color
				SColor bgColor = SColor(255, 128, 128, 255);

				// Draw background rectangle
				recti r = recti(levelAreas[i]);
				vector2di diff = vector2di(windowSize.Height / 50, windowSize.Height / 75);
				vector2di offset(x, 0);

				r.UpperLeftCorner += diff + offset;
				r.LowerRightCorner += -diff + offset;

				IGUIImage* image = guienv->addImage(r);
				image->setImage(rectangleTexture);
				image->setScaleImage(true);
				image->setColor(SColor(255, 255, 128, 128));
			}

			// Foreground color
			SColor fgColor = i == currentIndex ? SColor(255, 255, 255, 0) : SColor(255, 255, 255, 255);

			// Compute right animated position
			levelAreas[i].UpperLeftCorner.X += x;
			levelAreas[i].LowerRightCorner.X += x;

			// Create text to be drawn
			IGUIStaticText* text = guienv->addStaticText(levelTitles[i].c_str(), levelAreas[i]);
			text->setOverrideFont(font);
			text->setOverrideColor(fgColor);
			text->setTextAlignment(i == 1 ? EGUIA_LOWERRIGHT : EGUIA_CENTER, EGUIA_CENTER);
		}
	}

	// Draw mouse pointer
	recti r = utility::getSourceRect(mouse) + EventManager::singleton->mousePosition;
	IGUIImage* image = guienv->addImage(r);
	image->setImage(mouse);
}