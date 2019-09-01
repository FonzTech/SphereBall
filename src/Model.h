#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <unordered_map>
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

class Model
{
public:
	std::unordered_map<u32, ITexture*> textures;
	IAnimatedMesh* mesh;
	vector3df position;
	vector3df rotation;
	vector3df scale;
	s32 material;
	f32 currentFrame;

	// Constructor
	Model(Model* model);
	Model(IAnimatedMesh* mesh);

	// Texture adder for layer
	void addTexture(u32 layer, ITexture* texture);
};

#endif // MODEL_H