#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <unordered_map>
#include <irrlicht.h>

using namespace std;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

class Model
{
public:
	unordered_map<u32, ITexture*> textures;
	IAnimatedMesh* mesh;
	vector3df position;
	vector3df rotation;
	vector3df scale;
	s32 material;

	// Constructor
	Model(IAnimatedMesh* mesh);

	// Texture adder for layer
	void addTexture(u32 layer, ITexture* texture);
};

#endif // MODEL_H