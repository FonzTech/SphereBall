#ifndef PLAYER_H
#define PLAYER_H

#define STATE_WALKING	0
#define STATE_DEAD		1

#include "GameObject.h"
#include "Alarm.h"
#include "ShaderCallback.h"

class Player : public GameObject
{
protected:

	// State
	s8 state;

	void walk();
	void die();
	void dead();

	// Shader variables
	s32 customShader;
	matrix4 transformMatrix;

	// Motion
	s8 direction;
	s8 moving;
	s8 falling;
	f32 breathing;
	f32 breathingSpeed;
	std::unique_ptr<vector3df> fallLine;

	// Alarms
	std::unique_ptr<Alarm> dieAlarm;
	std::unique_ptr<Alarm> popAlarm;

	// Custom collision check function
	std::function<bool(const GameObject* go)> coinCollisionCheck;
	std::function<bool(const GameObject* go)> spikesCollisionCheck;

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