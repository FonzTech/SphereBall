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
	mouse = driver->getTexture("textures/mouse.png");

	// Create titles for options
	optionTitles[0] = L"Start Game";
	optionTitles[1] = L"Level Editor";
	optionTitles[2] = L"Options";
	optionTitles[3] = L"Exit";

	optionTitles[4] = L"Resolution";
	optionTitles[5] = L"SFX Volume";
	optionTitles[6] = L"Music Volume";
	optionTitles[7] = L"Back";

	// Load sound
	sounds[KEY_SOUND_SELECT] = SoundManager::singleton->getSound(KEY_SOUND_SELECT);
	sounds[KEY_SOUND_SELECT]->setRelativeToListener(true);

	// Initialize members
	currentSection = 0;
	currentIndex = -1;
	animation = 0;
}

void MainMenu::update()
{
	// Get event manager and window size
	dimension2di windowSize = utility::getWindowSize<s32>(driver);

	// Temporary current index
	s8 tmpIndex = -1;

	// Calculate vertical sections for menu
	for (s8 i = 0; i < 8; ++i)
	{
		// Calculate fixed horizonal sections for entries
		s32 x1 = (s32)((f32)windowSize.Width * 0.25f) + (i >= 4 ? windowSize.Width : 0);
		s32 x2 = (s32)((f32)windowSize.Width * 0.75f) + (i >= 4 ? windowSize.Width : 0);

		// Calculate vertical position for entries
		s32 y1 = (s32)(windowSize.Height * (0.3f + 0.1f * (i % 4)));
		s32 y2 = (s32)(windowSize.Height * (0.4f + 0.1f * (i % 4)));
		optionsAreas[i] = recti(x1, y1, x2, y2);

		// Check mouse in area
		if (optionsAreas[i].isPointInside(EventManager::singleton->mousePosition))
		{
			tmpIndex = i;
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
		if (currentIndex == 0)
		{
			RoomManager::singleton->loadRoom("level_1");
		}
		else if (currentIndex == 2)
		{
			currentSection = 1;
		}
		else if (currentIndex == 3)
		{
			if (currentSection == 0)
			{
				RoomManager::singleton->isProgramRunning = false;
			}
			else
			{
				currentSection = 0;
			}
		}
	}

	// Animate sections
	if (currentSection == 0)
	{
		animation -= 0.0025f * deltaTime;
		if (animation <= 0)
			animation = 0;
	}
	else
	{
		animation += 0.0025f * deltaTime;
		if (animation >= 1)
			animation = 1;
	}
}

void MainMenu::draw()
{
	// Reposition camera
	Camera::singleton->position = position + vector3df(0, 40, -100);
	Camera::singleton->lookAt = position;

	// Get animated value
	f32 animatedValue = utility::getCubicBezierAt(*(&vector2df(0, 1)), *(&vector2df(1, 0)), animation).X;

	// Draw titles
	for (u8 i = 0; i < 8; ++i)
	{
		// Get right color
		SColor color = i % 4 == currentIndex ? SColor(255, 255, 255, 0) : SColor(255, 0, 0, 0);

		// Compute right animated position
		s32 x = (s32)(animatedValue * (f32)optionsAreas[i].getWidth() * -2.0f);
		optionsAreas[i].UpperLeftCorner.X += x;
		optionsAreas[i].LowerRightCorner.X += x;

		// Create text to be drawn
		IGUIStaticText* text = guienv->addStaticText(optionTitles[i], optionsAreas[i]);
		text->setOverrideFont(font);
		text->setOverrideColor(color);
		text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
	}

	// Draw mouse pointer
	recti r = utility::getSourceRect(mouse) + EventManager::singleton->mousePosition;
	IGUIImage* image = guienv->addImage(r);
	image->setImage(mouse);
}