#ifndef EDITOR_H
#define EDITOR_H

#include "Hud.h"
#include "EventManager.h"

class Editor : public Hud
{
public:

	// Constructor
	Editor();

	// Mandatory methods
	void update();

	void drawHud();

	// Create specialized instance
	static std::shared_ptr<Editor> createInstance(const json &jsonData);
};

#endif // EDITOR_H