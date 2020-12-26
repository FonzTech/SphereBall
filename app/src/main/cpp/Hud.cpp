#include "Hud.h"
#include "EventManager.h"
#include "SharedData.h"

Hud::Hud() : GameObject()
{
	// Initialize font to null
	font = nullptr;

	// Load textures
	mouse = driver->getTexture("textures/gui_mouse.png");
	rectangleTexture = driver->getTexture("textures/gui_rectangle.png");

	// Fade out animation
	SharedData::singleton->startFade(false, nullptr);
}

void Hud::update()
{
	// Get window size
	windowSize = Utility::getWindowSize<f32>(driver);
	mousePosition = adjustResolutionAndGetMouse();
}

void Hud::draw()
{
	// Render on GUI RTT
	driver->setRenderTarget(SharedData::singleton->guiRtt);

	// Load font if required
	if (font == nullptr)
	{
		font = guienv->getFont(hudSize.Width > 1024 ? "fonts/titles.xml" : "fonts/titles_small.xml");
	}

	// Draw GUI
	drawHud();

	// Draw mouse pointer
	{
		recti r = Utility::getSourceRect(mouse) + mousePosition;
		IGUIImage* image = guienv->addImage(r);
		image->setImage(mouse);
	}

	// Draw GUI and Reset RTT
	guienv->drawAll();
	guienv->clear();

	driver->setRenderTargetEx(SharedData::singleton->sceneRtts[0], 0);
}

vector2di Hud::adjustResolutionAndGetMouse()
{
	s32 videoMode = Utility::getVideoMode(device);
	hudSize = videoMode == -1 ? dimension2du((u32)windowSize.X, (u32)windowSize.Y) : device->getVideoModeList()->getVideoModeResolution(videoMode);

	vector2df pos(vector2df((f32)EventManager::singleton->mousePosition.X, (f32)EventManager::singleton->mousePosition.Y) / windowSize * vector2df((f32)hudSize.Width, (f32)hudSize.Height));
	return mousePosition = vector2di((s32)pos.X, (s32)pos.Y);
}