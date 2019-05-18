#include "SkyBox.h"
#include "Camera.h"

const std::string SkyBox::FRAMES[] = {
	"right", "back", "left", "front", "down", "up"
};
const f32 SkyBox::DEFAULT_MODEL_ANGLE = 90.0f;
const f32 SkyBox::FACE_DISTANCE = 1000.0f;
const f32 SkyBox::FACE_BIAS = 11.0f;

std::shared_ptr<SkyBox> SkyBox::createInstance(const json &jsonData)
{
	std::string fname;
	jsonData.at("optional").at("texture").get_to(fname);

	return std::make_shared<SkyBox>(fname);
}

SkyBox::SkyBox(const std::string &textureName) : GameObject()
{
	// Load mesh
	IAnimatedMesh* mesh = smgr->getMesh("models/plane.obj");

	// Create all the six sides
	for (int i = 0; i < 6; ++i)
	{
		// Load texture
		const std::string texturePath = "textures/skybox_" + textureName + "_" + FRAMES[i] + ".jpg";
		ITexture* texture = driver->getTexture(texturePath.c_str());

		// Duplicate model and textures for sides
		std::shared_ptr<Model> model = std::make_shared<Model>(smgr->getMesh("models/plane.obj"));
		model->scale = vector3df(FACE_DISTANCE * 0.1f);
		model->addTexture(0, texture);
		models.push_back(model);
	}

	// Initialize variable
	angle = 0;
}

void SkyBox::update()
{
	// Move skybox along camera
	position.X = Camera::singleton->position.X;

	// Make skybox rotate
	angle += 0.0005f * deltaTime;
}

void SkyBox::draw()
{
	// Adjust all the six faces
	for (int i = 0; i < models.size(); ++i)
	{
		// Get model
		std::shared_ptr<Model> model = models.at(i);

		// Faces perpendicular to the horizontal plane
		if (i < 4)
		{
			f32 procAngle = angle + i * 90;
			f32 angleInRadians = degToRad(procAngle);

			f32 x = (f32)(std::cos(angleInRadians) * (FACE_DISTANCE - FACE_BIAS));
			f32 z = (f32)(std::sin(angleInRadians) * (FACE_DISTANCE - FACE_BIAS));

			model->position = position + vector3df(x, 0, z);
			model->rotation = vector3df(0, -procAngle + DEFAULT_MODEL_ANGLE, 0);
		}
		// Faces parallel to the horizontal plane
		else
		{
			model->position = position + vector3df(0, (FACE_DISTANCE - FACE_BIAS) * (i == 4 ? -1 : 1), 0);
			model->rotation = vector3df(i == 4 ? 90.0f : -90.0f, -angle + DEFAULT_MODEL_ANGLE, 0);
		}
	}
}