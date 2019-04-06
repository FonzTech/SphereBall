#ifndef COIN_H
#define COIN_H

#include "GameObject.h"
#include "EventManager.h"
#include "ShaderCallback.h"
#include <SFML/Audio.hpp>

class Coin : public GameObject
{
protected:
	f32 angle;

	// Plane model
	std::shared_ptr<Model> planeModel;

	// Sparkle shader
	s32 customShader;

public:

	// Specialized variables
	bool notPicked;

	// Constructor
	Coin();

	// Mandatory methods
	void update();
	void draw();

	// Specialized methods
	void pick();

	// Create specialized instance
	static std::shared_ptr<Coin> createInstance(const json &jsonData);

	// ShaderCallBack
	class SpecializedShaderCallback : public ShaderCallback
	{
	protected:
		Coin* coin;

	public:
		SpecializedShaderCallback(Coin* coin);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	};
};

#endif // COIN_H