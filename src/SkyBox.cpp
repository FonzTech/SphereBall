#include "SkyBox.h"
#include "Camera.h"

using nlohmann::json;

const std::string SkyBox::FRAMES[] = {
	"top", "bottom", "left", "right", "front", "back"
};

std::shared_ptr<SkyBox> SkyBox::createInstance(const nlohmann::json &jsonData)
{
	std::string fname;
	jsonData.at("optional").at("texture").get_to(fname);

	return std::make_shared<SkyBox>(fname);
}

SkyBox::SkyBox(const std::string &textureName) : GameObject()
{
	// Set game object index
	gameObjectIndex = KEY_GOI_SKYBOX;

	// Create dummy model
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->material = getCommonBasicMaterial(EMT_SOLID);
	models.push_back(model);

	// Create all the six sides
	for (int i = 0; i < 6; ++i)
	{
		// Load texture
		const std::string texturePath = "textures/skybox_" + textureName + "_" + FRAMES[i] + ".jpg";
		ITexture* texture = driver->getTexture(texturePath.c_str());
		model->addTexture(i, texture);
	}

	// Initialize variable
	angle = 0;
}

void SkyBox::update()
{
	// Move skybox along camera
	// position.X = Camera::singleton->position.X;

	// Make skybox rotate
	models.at(0)->rotation.Y += 0.0005f * deltaTime;
}

void SkyBox::draw()
{
}