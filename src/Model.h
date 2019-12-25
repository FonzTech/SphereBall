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
	/**
		This structure indicates the normal map texture index in the "textures" map of the model
		and the light power to be applied for shading. A value of 0 indicates no normal map enabled.
		So avoid binding normal map texture to slot 0.
	*/
	struct
	{
		s32 textureIndex = 0;
		f32 lightPower = 1.5f;
	} normalMapping;

	std::unordered_map<u32, ITexture*> textures;
	IAnimatedMesh* mesh;
	aabbox3df boundingBox;

	vector3df position;
	vector3df rotation;
	vector3df scale;
	s32 material;
	f32 currentFrame;

	// Constructor
	Model();
	Model(Model* model);
	Model(IAnimatedMesh* mesh);

	// Texture adder for layer
	void addTexture(u32 layer, ITexture* texture);
};

#endif // MODEL_H