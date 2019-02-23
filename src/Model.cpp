#include "Model.h"

Model::Model(IAnimatedMesh* mesh)
{
	// Assign mesh
	this->mesh = mesh;

	// Initialize members
	textures = unordered_map<u32, ITexture*>();
	material = -1;
}

void Model::addTexture(u32 layer, ITexture* texture)
{
	textures[layer] = texture;
}