#ifndef MAINMENU_H
#define MAINMENU_H

#include "GameObject.h"
#include "EventManager.h"

using namespace std;

class MainMenu : public GameObject
{
protected:
	// Font for menu entriess
	IGUIFont* font;

	// Texture for mouse
	ITexture* mouse;

	// Strings for option titles
	const wchar_t* optionTitles[8];

	// Members for option areas
	recti optionsAreas[8];
	f32 animation;

	// Current selected entry index
	s8 currentSection;
	s8 currentIndex;

public:
	// Constructor
	MainMenu();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static shared_ptr<MainMenu> createInstance(const json &jsonData);
};

#endif // MAINMENU_H