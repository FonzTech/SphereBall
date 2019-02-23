#include "SkyBox.h"
#include "Camera.h"

shared_ptr<SkyBox> SkyBox::createInstance(const json &jsonData)
{
	std::string fname;
	jsonData.at("optional").at("texture").get_to(fname);

	std::string dir = "textures/";
	return make_shared<SkyBox>(dir.append(fname));
}

SkyBox::SkyBox(std::string &textureFilename) : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh = smgr->getMesh("models/skybox.obj");

	// Load texture
	ITexture* texture = driver->getTexture(textureFilename.c_str());

	// Create model for player
	shared_ptr<Model> model = make_shared<Model>(mesh);
	model->addTexture(0, texture);
	models.push_back(model);

	// Initialize variable
	angle = 0;
}

void SkyBox::update()
{
	angle += 0.0005f * deltaTime;
}

void SkyBox::draw()
{
	shared_ptr<Model> model = models.at(0);
	model->position.X = Camera::singleton->position.X;
	model->rotation = vector3df(0, angle, 0);
}