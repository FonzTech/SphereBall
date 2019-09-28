#include "Model.h"

Model::Model()
{
	textures = std::unordered_map<u32, ITexture*>();
	material = -1;
	currentFrame = 0.0f;
}

Model::Model(IAnimatedMesh* mesh) : Model()
{
	// Assign mesh and bounding box
	this->mesh = mesh;
	boundingBox = mesh->getBoundingBox();
}

Model::Model(Model* model)
{
	// Assign mesh
	this->mesh = model->mesh;

	// Initialize members
	textures = std::unordered_map<u32, ITexture*>(model->textures);
	material = model->material;

	currentFrame = 0.0f;
}

void Model::addTexture(u32 layer, ITexture* texture)
{
	textures[layer] = texture;
}