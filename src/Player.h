#ifndef PLAYER_H
#define PLAYER_H

#define STATE_WALKING	0
#define STATE_DEAD		1
#define STATE_EXITED	2
#define STATE_TIME_OUT	3
#define STATE_BURNED	4

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
	f32 noiseFactor;

	void walk();
	void die();
	void dead();
	void exited();

	void resetBreathing();

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

	// Behaviour
	f32 fireFactor;

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

	/*
		Check if the player is walking / standing on a solid platform, if it's jumping or
		if it's falling. Jumping and falling state are almost the same, so if you have to
		check whether the player is not touching the solid ground, just check the return
		value for any value different from zero.

		@return 32-bit signed integer.
			- 0: not falling.
			- 1: player hasn't reeached the fall line and is jumping upwards.
			- 2: player has reached the fall line or is falling downwards.
	*/
	s32 getJumpingState();

	/*
		Add speed vector to the player.
		
		@param motion the motion vector to add to the current player's one.
		@param fallLine the point describing the threshold at which the player begin to fall.
	*/
	void addSpeed(const vector3df & motion, const vector3df & fallLine);

	// Create specialized instance
	static std::shared_ptr<Player> createInstance(const json &jsonData);
};

#endif // PLAYER_H