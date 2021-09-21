#ifndef EDITOR_H
#define EDITOR_H

#include "Hud.h"
#include "EventManager.h"

class Editor : public Hud
{
protected:
	vector3df velocity;
	u8 direction;
	vector3df cursorPos;
	vector3df zoom;
	vector2df snap;

public:

	static std::shared_ptr<GameObject> singleton;

	// Constructor
	Editor();

	// Mandatory methods
	void update();
	void drawHud();
	void postUpdate();

	// Create specialized instance
	static std::shared_ptr<Editor> createInstance(const nlohmann::json &jsonData);
};

#endif // EDITOR_H