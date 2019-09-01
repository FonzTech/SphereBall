#ifndef PLAYER_H
#define PLAYER_H

#define STATE_WALKING	0
#define STATE_DEAD		1
#define STATE_EXITED	2

#include "GameObject.h"
#include "Alarm.h"
#include "ShaderCallback.h"

class Player : public GameObject
{
protected:

	// Constants
	const static f32 breathingDelta;

	// State
	s8 state;

	void walk();
	void die();
	void dead();
	void exited();

	// Shader variables
	s32 customMaterial;
	matrix4 transformMatrix;

	// Motion
	s8 direction;
	s8 moving;
	s8 falling;
	f32 breathing;
	f32 breathingSpeed;
	std::unique_ptr<vector3df> fallLine;
	vector3df exitedPosition;

	// Alarms
	std::unique_ptr<Alarm> dieAlarm;
	std::unique_ptr<Alarm> popAlarm;

	// Custom collision check function
	std::unordered_map<std::string, std::function<bool(GameObject* go)>> collisionChecks;

	// Update transform matrix
	void updateTransformMatrix();

	// ShaderCallBack
	class SpecializedShaderCallback : public ShaderCallback
	{
	protected:
		Player* player;

	public:
		SpecializedShaderCallback(Player* coin);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	};

public:

	// Constructor
	Player();

	// Mandatory methods
	void update();
	void draw();

	// Create specialized instance
	static std::shared_ptr<Player> createInstance(const json &jsonData);
};

#endif // PLAYER_H