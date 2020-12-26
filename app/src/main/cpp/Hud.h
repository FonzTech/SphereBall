#ifndef HUD_H
#define HUD_H

#include "GameObject.h"

class Hud : public GameObject
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

	// Resolution adjuster
	vector2di adjustResolutionAndGetMouse();

public:

	// Constructor and deconstructor
	Hud();

	// Mandatory methods
	virtual void update();
	void draw();

	virtual void drawHud() = 0;
};

#endif // HUD_H