#ifndef UTILITY_H
#define UTILITY_H

class GameObject;

#include <memory>
#include <irrlicht.h>
#include <nlohmann/json.hpp>

using namespace std;

using namespace irr;
using namespace core;
using namespace video;
using namespace gui;

using nlohmann::json;

namespace utility
{
	// Structure which acts as a container to hold collision results
	struct Collision
	{
		shared_ptr<GameObject> gameObject = nullptr;
		aabbox3df mainBoundingBox, otherBoundingBox;
	};

	// Transform axis-aligned bounding box by point
	void transformAABBox(aabbox3d<f32> &dest, const vector3df &translate = vector3df(0, 0, 0), const vector3df &rotation = vector3df(0, 0, 0), const vector3df &scale = vector3df(1, 1, 1));

	// Get horizontal bounding box
	void getHorizontalAABBox(aabbox3df &box, aabbox3df &dest, f32 align, f32 verScale = 1);

	// Get vertical bounding box
	void getVerticalAABBox(aabbox3df &box, aabbox3df &dest, f32 align, f32 horScale = 1);

	// Get window size
	dimension2di getWindowSize(IVideoDriver* driver);

	// Get text rectangle within screen space
	rect<s32> getPositionInScreenSpace(IVideoDriver* driver, const f32 x, const f32 y, const s32 width = 1024, const s32 height = 1024, IGUIFont* font = nullptr, const wchar_t* text = nullptr, const f32 alignX = 0.0f, const f32 alignY = 0.0f);

	// Get full source rect from texture
	recti getSourceRect(ITexture* texture);

	// Generate Bezier's Curve
	const vector2df getCubicBezierAt(const vector2df& a, const vector2df& b, const f32 t);

	// Assign common room data for GameObject
	void assignGameObjectCommonData(shared_ptr<GameObject>& gameObject, json& commonData);
}

#endif // UTILITY_H