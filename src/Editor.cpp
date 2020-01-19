#include "Editor.h"
#include "RoomManager.h"
#include "SharedData.h"
#include "Camera.h"

std::shared_ptr<GameObject> Editor::singleton = nullptr;

std::shared_ptr<Editor> Editor::createInstance(const json &jsonData)
{
	return std::make_shared<Editor>();
}

Editor::Editor() : Hud()
{
	// Load plane model with grid texture
	IAnimatedMesh* mesh = Utility::getMesh(smgr, "models/plane.obj");
	ITexture* texture = driver->getTexture("textures/grid.png");

	// Create model for grid
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = getCommonBasicMaterial(EMT_SOLID);
	model->scale = vector3df(4, 4, 1);
	models.push_back(model);

	// Initialize variables
	direction = 0;
	zoom = vector3df(1, 0, 0);

	// Fade out animation
	SharedData::singleton->startFade(false, nullptr);
}

void Editor::update()
{
	// Call parent method
	Hud::update();

	// Check for zooming
	if (EventManager::singleton->mouseWheel != 0)
	{
		zoom.Y = EventManager::singleton->mouseWheel > 0 ? -0.1f : 0.1f;
		zoom.Z = zoom.Y;
	}

	// Limit zoom velocity
	if (zoom.Z > 0.0f)
	{
		zoom.Y -= 0.001f * deltaTime;
		if (zoom.Y < 0)
		{
			zoom.Y = 0;
		}
	}
	else if (zoom.Z < 0.0f)
	{
		zoom.Y += 0.001f * deltaTime;
		if (zoom.Y > 0)
		{
			zoom.Y = 0;
		}
	}

	// Affect zoom with speed
	zoom.X += zoom.Y;

	// Limit zoom
	if (zoom.X < 0.3f)
	{
		zoom.X = 0.3f;
	}
	else if (zoom.X > 4.0f)
	{
		zoom.X = 4.0f;
	}

	// Check for horizontal panning
	if (EventManager::singleton->keyStates[KEY_KEY_W] >= KEY_PRESSED)
	{
		velocity.Y += 0.01f * deltaTime;
		direction = direction & 12 | 1;

		if (velocity.Y > 4)
		{
			velocity.Y = 4;
		}
	}
	else if (EventManager::singleton->keyStates[KEY_KEY_S] >= KEY_PRESSED)
	{
		velocity.Y += -0.01f * deltaTime;
		direction = direction & 12 | 2;

		if (velocity.Y < -4)
		{
			velocity.Y = -4;
		}
	}
	else if (direction & 1)
	{
		velocity.Y -= 0.025f * deltaTime;
		if (velocity.Y < 0)
		{
			velocity.Y = 0;
		}
	}
	else if (direction & 2)
	{
		velocity.Y += 0.025f * deltaTime;
		if (velocity.Y > 0)
		{
			velocity.Y = 0;
		}
	}

	// Check for vertical panning
	if (EventManager::singleton->keyStates[KEY_KEY_A] >= KEY_PRESSED)
	{
		velocity.X += -0.01f * deltaTime;
		direction = direction & 3 | 4;

		if (velocity.X < -4)
		{
			velocity.X = -4;
		}
	}
	else if (EventManager::singleton->keyStates[KEY_KEY_D] >= KEY_PRESSED)
	{
		velocity.X += 0.01f * deltaTime;
		direction = direction & 3 | 8;

		if (velocity.X > 4)
		{
			velocity.X = 4;
		}
	}
	else if (direction & 4)
	{
		velocity.X += 0.025f * deltaTime;
		if (velocity.X > 0)
		{
			velocity.X = 0;
		}
	}
	else if (direction & 8)
	{
		velocity.X -= 0.025f * deltaTime;
		if (velocity.X < 0)
		{
			velocity.X = 0;
		}
	}

	// Affect position with speed
	position += velocity;

	// Set correct camera position
	Camera::singleton->lookAt = position;
	Camera::singleton->position = position + vector3df(0, 0, -100 * zoom.X);

	// Compute snapped position
	snap = vector2df(std::floorf(cursorPos.X / 10) * 10, std::floorf(cursorPos.Y / 10) * 10);
}

void Editor::postUpdate()
{
	// Get 3D ray
	ISceneCollisionManager* colmgr = smgr->getSceneCollisionManager();
	line3d<f32> ray = colmgr->getRayFromScreenCoordinates(device->getCursorControl()->getPosition());

	// And intersect the ray with a plane around the node facing towards the camera.
	plane3df plane(position, vector3df(0, 0, -1));
	vector3df mousePosition;
	if (plane.getIntersectionWithLine(ray.start, ray.getVector(), mousePosition))
	{
		mousePosition.Z = 0;
		cursorPos = mousePosition;
	}

	// Move cursor
	models.at(0)->position = cursorPos;
}

void Editor::drawHud()
{
	// Create text to be drawn
	IGUIStaticText* text = guienv->addStaticText(L"Level Editor", recti(0, 0, hudSize.Width, 128));
	text->setOverrideFont(font);
	text->setOverrideColor(SColor(255, 128, 255, 0));
	text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);

	// Coordinates
	for (u8 i = 0; i < 2; ++i)
	{
		std::wstring str;

		if (i)
		{
			str = L"Y: ";
			str += std::to_wstring((s32)(snap.Y));
		}
		else
		{
			str = L"X: ";
			str += std::to_wstring((s32)(snap.X));
		}

		s32 h = i * 80;
		IGUIStaticText* text = guienv->addStaticText(str.c_str(), recti(16, h, 512, 96 + h));
		text->setOverrideFont(font);
		text->setOverrideColor(SColor(255, 128, 255, 0));
		text->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	}
}