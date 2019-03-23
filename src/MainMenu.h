#ifndef MAINMENU_H
#define MAINMENU_H

#include "GameObject.h"
#include "EventManager.h"

class MainMenu : public GameObject
{
protected:
	// Font for menu entriess
	IGUIFont* font;

	// Textures
	ITexture* mouse;
	ITexture* rectangleTexture;

	// Window size
	dimension2di windowSize;

	// Strings for text strings
	std::vector<std::wstring> optionTitles;
	std::vector<std::wstring> levelTitles;

	// Members for text ares
	std::vector<recti> optionsAreas;
	std::vector<recti> levelAreas;
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