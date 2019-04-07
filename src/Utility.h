#ifndef UTILITY_H
#define UTILITY_H

#include <memory>
#include <irrlicht.h>
#include <SFML/Audio.hpp>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace irr;
using namespace core;
using namespace video;
using namespace gui;

using nlohmann::json;

namespace utility
{
	// Transform axis-aligned bounding box by point
	const void transformAABBox(aabbox3d<f32> &dest, const vector3df &translate = vector3df(0, 0, 0), const vector3df &rotation = vector3df(0, 0, 0), const vector3df &scale = vector3df(1, 1, 1));

	// Get horizontal bounding box
	const void getHorizontalAABBox(const aabbox3df &source, aabbox3df &dest, const f32 align, const f32 verScale = 1);

	// Get vertical bounding box
	const void getVerticalAABBox(const aabbox3df &source, aabbox3df &dest, const f32 align, const f32 horScale = 1);

	// Get window size
	template <typename T>
	const vector2d<T> getWindowSize(IVideoDriver* driver)
	{
		// Get exposed video data
		const SExposedVideoData evd = driver->getExposedVideoData();

		// Get window size
		#ifdef __linux__
		// Not implemented yet

		#elif _WIN32
		HWND hwnd = reinterpret_cast<HWND>(evd.OpenGLWin32.HWnd);
		RECT lpRect;
		if (GetClientRect(hwnd, &lpRect))
		{
			return vector2d<T>((T)(lpRect.right - lpRect.left), (T)(lpRect.bottom - lpRect.top));
		}

		#endif

		return vector2d<T>();
	}

	// Get text rectangle within screen space
	const rect<s32> getPositionInScreenSpace(const IVideoDriver* driver, const f32 x, const f32 y, const s32 width = 1024, const s32 height = 1024, IGUIFont* font = nullptr, const wchar_t* text = nullptr, const f32 alignX = 0.0f, const f32 alignY = 0.0f);

	// Convert Irrlicht 3D vector to SFML 3D vector
	const sf::Vector3f irrVectorToSf(const vector3df& v);

	// Get full source rect from texture
	const recti getSourceRect(const ITexture* texture);

	// Generate Bezier's Curve
	const vector2df getCubicBezierAt(const vector2df& a, const vector2df& b, const f32 t);

	// Vector rotation around a pivot
	void rotateVectorAroundPivot(const vector3df & input, vector3df & out, const vector3df & pivot, const vector3df & angles);
}

#endif // UTILITY_H