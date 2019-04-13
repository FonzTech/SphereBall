#include "Utility.h"
#include "GameObject.h"
#include <string>

#ifdef __linux__ 
// Not implemented yet

#elif _WIN32
#include <Windows.h>

#endif

namespace utility
{
	const void transformAABBox(aabbox3d<f32> &dest, const vector3df &translation, const vector3df &rotation, const vector3df &scale)
	{
		matrix4 trans;
		trans.setTranslation(translation);
		trans.setScale(scale);
		trans.transformBoxEx(dest);
	}

	const void getHorizontalAABBox(const aabbox3df &source, aabbox3df &dest, const f32 align, const f32 verScale)
	{
		utility::transformAABBox(dest, vector3df(source.getExtent().X / 2 * align, 0, 0), vector3df(0, 0, 0), vector3df(0.05f, 1, 1) * verScale);
	}

	const void getVerticalAABBox(const aabbox3df &source, aabbox3df &dest, const f32 align, const f32 horScale)
	{
		utility::transformAABBox(dest, vector3df(0, source.getExtent().Y / 2 * align, 0), vector3df(0, 0, 0), vector3df(1, 0.05f, 1) * horScale);
	}

	const rect<s32> getPositionInScreenSpace(IVideoDriver* driver, const f32 x, const f32 y, const s32 width, const s32 height, IGUIFont* font, const wchar_t* text, const f32 alignX, const f32 alignY)
	{
		// Get window size
		const dimension2di windowSize = getWindowSize<s32>(driver);

		// Variables for transform
		s32 tx = 0, ty = 0;

		// Align if required data is provided
		if (font != nullptr)
		{
			dimension2du textSize = font->getDimension(text);
			tx = (s32)((f32)(textSize.Width) * alignX);
			ty = (s32)((f32)(textSize.Height) * alignY);
		}

		// Compute final position
		s32 dx = (s32)((f32)(windowSize.Width) * x + tx);
		s32 dy = (s32)((f32)(windowSize.Height) * y + ty);

		// Return final rect
		return rect<s32>(dx, dy, dx + width, dy + height);
	}

	const sf::Vector3f irrVectorToSf(const vector3df& v)
	{
		return sf::Vector3f(v.X, v.Y, v.Z);
	}

	const recti getSourceRect(const ITexture* texture)
	{
		return recti(vector2di(0, 0), vector2di(texture->getSize().Width, texture->getSize().Height));
	}

	const vector2df getCubicBezierAt(const vector2df& a, const vector2df& b, const f32 t)
	{
		return 3.0f * std::pow(1.0f - t, 2.0f) * t * a + 3.0f * (1.0f - t) * std::pow(t, 2.0f) * b + pow(t, 3.0f);
	}

	void rotateVectorAroundPivot(const vector3df & input, vector3df & out, const vector3df & pivot, const vector3df & angles)
	{
		quaternion q = quaternion::quaternion(angles);
		out = vector3df(q * (input - pivot) + pivot);
	}
}