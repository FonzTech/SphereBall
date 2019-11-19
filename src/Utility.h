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
using namespace scene;
using namespace video;
using namespace gui;

using nlohmann::json;

class Utility
{
protected:
	static s32 videoModeCount;
	static s32 videoModeCurrent;

	/**
		Check whether the requested mesh is loaded into the engine's mesh cache. If so, just load
		it from the cache. If not, check if the mesh is skinned (*.x meshes) to automatically create
		its tangent space. Other mesh types are not taken into account, for performance reasons.
	*/
	static IAnimatedMesh* getMeshWithTangents(ISceneManager* smgr, const std::string& path);

public:

	/**
		Check whether a string begins with a certain prefix (a token, to say) or not.

		@param s the string where to check the prefix on.
		@param prefix the prefix to be checked.

		@return "true" if "s" starts with "prefix", otherwise "false".
	*/
	static const bool startsWith(const std::string& s, const std::string& prefix);

	/**
		Check whether a string ends with a certain suffix (a token, to say) or not.

		@param s the string where to check the suffix on.
		@param suffix the suffix to be checked.

		@return "true" if "s" ends with "suffix", otherwise "false".
	*/
	static const bool endsWith(const std::string& s, const std::string& suffix);

	/**
		Compute the modulus for the given numbers. This method is added because
		the standard modulo operator (%) does not work as expected for negative
		values.

		@param x the dividend number.
		@param y the divisor number.

		@return the remainder for the division between x and y.
	*/
	static const s32 mod(const s32 x, const s32 y);

	/**
		Get currently set video mode. If the current mode is invalid, it will
		be automatically set to -1, which means the size of the app's window.

		@param device the Irrlicht's device obtained during the engine setup.

		@return signed integer representing the current video mode index.
	*/
	static const s32 getVideoMode(IrrlichtDevice* device);

	/**
		Step the video mode index N times, defined as argument. Video mode change
		works this way, by wrapping values around the maximum video mode count,
		to prevent setting invalid video modes.

		@param value the step value.

		@return the currently set mode after the execution of this method.
	*/
	static const s32 stepVideoMode(const s32 value);

	/**
		Provide a proxy pattern for mesh loading. By default, Irrlicht supports certain type of file formats.
		Animated meshes, like x (DirectX), are huge compared to other static meshes. This problem is resolved
		by zipping these large files, then unzipping when requested. Irrlicht's resource management is preserved.
		Tangent space is created on DirectX meshes (*.x files), since they are skinned. Other types of meshes
		does not have any tangent space related informations.

		@param smgr the Irrlicht's Scene Manager obtained from EngineObject class and subclasses.
		@param name the file name, which can be a zip file or any file supported by the Irrlicht engine.

		@return pointer to the requested IAnimatedMesh on success, otherwise "nullptr" is returned.
	*/
	static IAnimatedMesh* getMesh(ISceneManager* smgr, const std::string& path);

	/**
		Get the system temporary directory. This is useful when working with temporary files which you don't need
		between different launches of the application. The implementation of this method is platform specific.
		If implementation for your system is not available, the current directory ("./") is returned.

		@return the string representing the temporary directory if available, otherwise "./" is returned.
	*/
	static const std::wstring getTempDirectory();

	/**
		Transform axis-aligned bounding box by point. This function does not return anything.

		@param dest the destination bounding box, passed by reference
		@param translate the translation, described by a vector, to perform on the bounding box. Default is a 0-vector.
		@param rotation the rotation, described by a vector, to perform on the bounding. box Default is a 0-vector.
		@param sclae the scale, described by a vector, to perform on the bounding box. Default is a unit-vector.
	*/
	static const void transformAABBox(aabbox3d<f32> &dest, const vector3df &translate = vector3df(0, 0, 0), const vector3df &rotation = vector3df(0, 0, 0), const vector3df &scale = vector3df(1, 1, 1));

	/**
		Get horizontal bounding box. This is merely an utility function to ease the work of getting
		a wall-like bounding box, translated horizontally by a factor, with a specified scaling.

		@param source the source bounding box, passed by reference.
		@param dest the destination bounding box, passed by reference, where to perform the transformation.
		@param align the alignment coefficient, where 0 is the center, -1 is the left side, 1 is the right side.
		@param verScale the vertical scaling for the bounding box, to prevent wrong collisions on rapid movements. Default is 1.
	*/
	static const void getHorizontalAABBox(const aabbox3df &source, aabbox3df &dest, const f32 align, const f32 verScale = 1);

	/**
		Get vertical bounding box. This is merely an utility function to ease the work of getting
		a floor-like bounding box, translated vertically by a factor, with a specified scaling.

		@param source the source bounding box, passed by reference.
		@param dest the destination bounding box, passed by reference, where to perform the transformation.
		@param align the alignment coefficient, where 0 is the center, -1 is the upper side, 1 is the lower side.
		@param horScale the horizontal scaling for the bounding box, to prevent wrong collisions on rapid movements. Default is 1.
	*/
	static const void getVerticalAABBox(const aabbox3df &source, aabbox3df &dest, const f32 align, const f32 horScale = 1);

	/**
		Get size of the window where the application is currently running. This is a template function, because
		this data is required to be stored in different data structures, which are used within the codebase. For
		example, <T> can be a <u32> or <f32>. Although they are always a subchild of <vector2d>. This function is
		platform-specific. Preprocessor directives are used to choose the right implementation at compile-time.

		@param T the data structure where to store the data to. It's required to have a constructor which takes two parameters.
		@param driver the Irrlicht Video Driver obtained from EngineObject class and subclasses.

		@return vector2d<T> filled with the current window size. If implementation is not found, then a 0-vector is returned.
	*/
	template <typename T>
	static const vector2d<T> getWindowSize(IVideoDriver* driver)
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

	/**
		Get text rectangle within screen space. This function is useful when text is required to be drawn. It provides a normalization
		for the screen coordinates, which ranges from a [ -1, 1 ] interval, where 0 is the middle of the axis, -1 is the left / top,
		1 is the right / bottom.

		@param driver the Irrlicht Video Driver obtained from EngineObject class and subclasses.
		@param x the x position.
		@param y the y position.
		@param width the width for the text.
		@param height the height for the text.
		@param font the font used to draw the text.
		@param text the actual text to be drawn.
		@param alignX the horizontal alignment.
		@param alignY the vertical alignment.

		@return rect<s32> (signed integer rect data structure) filled with text rect data.

		Currently
	*/
	static const rect<s32> getPositionInScreenSpace(IVideoDriver* driver, const f32 x, const f32 y, const s32 width = 1024, const s32 height = 1024, IGUIFont* font = nullptr, const wchar_t* text = nullptr, const f32 alignX = 0.0f, const f32 alignY = 0.0f);

	/**
		Convert Irrlicht 3D vector to SFML 3D vector. This is useful when working with spatial audio.

		@param v the 3-dimension flaoting point vector to be converted.

		@return vector of type sf::Vector3f, ready for use in SFML framework.
	*/
	static const sf::Vector3f irrVectorToSf(const vector3df& v);

	/**
		Get full source rect from texture. This function builds a <recti> data structure, ready for
		use in Irrlicht's routines.

		@param texture the texture where to perform the lookup on.

		@return a <recti> (signed integer rectangle) data structure filled with (0, 0, texture width, texture height).
	*/
	static const recti getSourceRect(const ITexture* texture);

	/**
		Compute the Cubic Bezier Curve, from specified points at a certain time. This is useful when animating
		a value, since any smoothing curve can be designed.

		@param a the starting point.
		@param b the ending point.
		@param t the "t" time , where 0 is the beginning and 1 is the ending.

		@return floating point 2D vector filled with (x,y) values for the curve at "t" time.
	*/
	static const vector2df getCubicBezierAt(const vector2df& a, const vector2df& b, const f32 t);

	/**
		 Rotate a vector around a pivot by a certain angles.

		 @param input the input vector to be rotated.
		 @param out the output vector to be rotated.
		 @param pivot the pivot point where to rotate the input around.
		 @param angles the rotation to be performed for desired axes.
	*/
	static void rotateVectorAroundPivot(const vector3df & input, vector3df & out, const vector3df & pivot, const vector3df & angles);

	/**
		Animate a floating point value to a target one. This function uses a fixed non-linear interpolation, more
		like a ease-out one. Animation is directly performed on the variable, which is passed by pointer.

		@param deltaTime the delta time used to shape the animation correctly in time.
		@param variableToAnimate the variable to animate, passed by pointer.
		@param targetValue the value to reach for the entire animation.
	*/
	static void animateFloatValue(const f32 deltaTime, f32* variableToAnimate, const f32 targetValue);
};

#endif // UTILITY_H