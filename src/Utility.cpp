#include "Utility.h"
#include "GameObject.h"

#include <filesystem>
#include <string>
#include <zipper/unzipper.h>

#ifdef __linux__ 
// Not implemented yet

#elif _WIN32
#include <Windows.h>

#endif

s32 Utility::videoModeCount = -1;
s32 Utility::videoModeCurrent = -1;

const bool Utility::startsWith(const std::string& s, const std::string& prefix)
{
	return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

const bool Utility::endsWith(const std::string& s, const std::string& suffix)
{
	return s.rfind(suffix) == (s.size() - suffix.size());
}

const s32 Utility::mod(const s32 x, const s32 y)
{
	return (x % y + y) % y;
}

const s32 Utility::getVideoMode(IrrlichtDevice* device)
{
	// Cache the value
	if (videoModeCount == -1)
	{
		videoModeCount = device->getVideoModeList()->getVideoModeCount();
	}

	// Check for validity
	if (videoModeCurrent < -1 || videoModeCurrent > videoModeCount)
	{
		videoModeCurrent = -1;
	}
	return videoModeCurrent;
}

const s32 Utility::stepVideoMode(const s32 value)
{
	return videoModeCurrent = mod(videoModeCurrent + value + 1, videoModeCount + 1) - 1;
}

IAnimatedMesh* Utility::getMesh(ISceneManager* smgr, const std::string& path)
{
	// Unzip if necessary
	if (endsWith(path, ".zip"))
	{
		// Get file name
		const auto name = std::filesystem::path(path).stem();

		// Get temporary directory path
		std::string dir = "./";
		{
			const std::wstring tmpDir = getTempDirectory();
			dir = std::string(tmpDir.cbegin(), tmpDir.cend());
		}

		// Check for 3D model file
		zipper::Unzipper unzipper(path);
		std::vector<zipper::ZipEntry> entries = unzipper.entries();
		for (auto& entry : entries)
		{
			const auto fpath = std::filesystem::path(entry.name);
			const auto entryName = fpath.stem().string();

			// If requested model is found
			if (entryName == name)
			{
				// Get full path for file
				const std::string file = dir + fpath.filename().string();

				// Check if file has been already extracted
				if (std::filesystem::exists(std::filesystem::path(file)))
				{
					#if NDEBUG || _DEBUG
					printf("ZIP Model - No need to extract %s\n", fpath.filename().string().c_str());
					#endif
				}
				else
				{
					// Unzip file
					unzipper.extractEntry(entry.name, dir);
				}

				// Set extracted mesh
				return getMeshWithTangents(smgr, file);
			}
		}

		// Close zip
		unzipper.close();
	}

	// Load the mesh
	return getMeshWithTangents(smgr, path);
}

IAnimatedMesh* Utility::getMeshWithTangents(ISceneManager* smgr, const std::string& path)
{
	const char* name = path.c_str();

	IAnimatedMesh* mesh;
	if (endsWith(path, ".x"))
	{
		if (!smgr->getMeshCache()->isMeshLoaded(name))
		{
			mesh = smgr->getMesh(path.c_str());
			((ISkinnedMesh*)mesh)->convertMeshToTangents();
			return mesh;
		}
	}

	return smgr->getMesh(name);
}

const std::wstring Utility::getTempDirectory()
{
#ifdef __linux__
	return L"/tmp/";

#elif _WIN32
	WCHAR path[MAX_PATH];
	GetTempPath(MAX_PATH, (LPTSTR)&path);

	return std::wstring(path);

#endif

	return L"./";
}

const void Utility::transformAABBox(aabbox3d<f32> &dest, const vector3df &translation, const vector3df &rotation, const vector3df &scale)
{
	matrix4 trans;
	trans.setTranslation(translation);
	trans.setScale(scale);
	trans.transformBoxEx(dest);
}

const void Utility::getHorizontalAABBox(const aabbox3df &source, aabbox3df &dest, const f32 align, const f32 verScale)
{
	transformAABBox(dest, vector3df(source.getExtent().X / 2 * align, 0, 0), vector3df(0, 0, 0), vector3df(0.05f, 1 * verScale, 1));
}

const void Utility::getVerticalAABBox(const aabbox3df &source, aabbox3df &dest, const f32 align, const f32 horScale)
{
	transformAABBox(dest, vector3df(0, source.getExtent().Y / 2 * align, 0), vector3df(0, 0, 0), vector3df(1 * horScale, 0.05f, 1));
}

const rect<s32> Utility::getPositionInScreenSpace(IVideoDriver* driver, const f32 x, const f32 y, const s32 width, const s32 height, IGUIFont* font, const wchar_t* text, const f32 alignX, const f32 alignY)
{
	// Get window size
	const dimension2di windowSize = Utility::getWindowSize<s32>(driver);

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

const sf::Vector3f Utility::irrVectorToSf(const vector3df& v)
{
	return sf::Vector3f(v.X, v.Y, v.Z);
}

const recti Utility::getSourceRect(const ITexture* texture)
{
	return recti(vector2di(0, 0), vector2di(texture->getSize().Width, texture->getSize().Height));
}

const vector2df Utility::getCubicBezierAt(const vector2df& a, const vector2df& b, const f32 t)
{
	return 3.0f * std::pow(1.0f - t, 2.0f) * t * a + 3.0f * (1.0f - t) * std::pow(t, 2.0f) * b + pow(t, 3.0f);
}

void Utility::rotateVectorAroundPivot(const vector3df & input, vector3df & out, const vector3df & pivot, const vector3df & angles)
{
	quaternion q = quaternion::quaternion(angles);
	out = vector3df(q * (input - pivot) + pivot);
}

void Utility::animateFloatValue(const f32 deltaTime, f32* variableToAnimate, const f32 targetValue)
{
	if (std::abs(*variableToAnimate) < std::abs(targetValue))
	{
		// Increment by fraction
		f32 diff = targetValue - *variableToAnimate;
		*variableToAnimate += diff * 0.0075f * deltaTime;

		// Jump to final value (to avoid animation slowdown)
		if (std::abs(diff) < 0.5f)
		{
			*variableToAnimate = targetValue;
		}
	}
}