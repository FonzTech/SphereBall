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

	// HUD and Window size
	vector2df windowSize;
	dimension2di hudSize;
	vector2di mousePosition;

	// Strings for text strings
	std::vector<std::wstring> optionTitles;
	std::vector<std::wstring> levelTitles;

	// Members for text ares
	std::vector<recti> optionsAreas;
	std::vector<recti> levelAreas;
	recti titleArea;
	f32 animation;

	// Current selected entry index
	s8 currentSection;
	s8 currentIndex;

	// Room jump variables
	std::string roomToLoad;

	// Jump Room callback function
	void jumpToLevel();

public:

	// Constructor
	MainMenu();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<MainMenu> createInstance(const json &jsonData);
};

#endif // MAINMENU_H