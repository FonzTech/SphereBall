#include "Editor.h"
#include "RoomManager.h"
#include "SharedData.h"

std::shared_ptr<Editor> Editor::createInstance(const json &jsonData)
{
	return std::make_shared<Editor>();
}

Editor::Editor() : Hud()
{
	// Fade out animation
	SharedData::singleton->startFade(false, nullptr);
}

void Editor::update()
{
	Hud::update();
}

void Editor::drawHud()
{
	// Create text to be drawn
	IGUIStaticText* text = guienv->addStaticText(L"Level Editor", recti(0, 0, hudSize.Width, 128));
	text->setOverrideFont(font);
	text->setOverrideColor(SColor(255, 128, 255, 0));
	text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
}