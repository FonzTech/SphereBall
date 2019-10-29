#include "Fire.h"

std::shared_ptr<Fire> Fire::createInstance(const json &jsonData)
{
	return std::make_shared<Fire>();
}

Fire::Fire() : GameObject()
{
	// Load mesh and texture for Exit model
	IAnimatedMesh* mesh = smgr->getMesh("models/bonfire.obj");
	ITexture* texture = driver->getTexture("textures/bonfire.png");

	// Create model for player
	std::shared_ptr<Model> model = std::make_shared<Model>(mesh);
	model->addTexture(0, texture);
	model->material = COMMON_EMT_SOLID;
	models.push_back(model);

	// Create fire particle
	createFileParticle();
}

Fire::~Fire()
{
	// Drop the pointer, so resources can be released
	particleSystem->drop();
	particleSystem = nullptr;
}

void Fire::update()
{
	// Add particle system scene node to scene
	smgr->getRootSceneNode()->addChild(particleSystem);
}

void Fire::draw()
{
	// Bonfire model
	std::shared_ptr<Model> model = models.at(0);
	model->position = position;
}

void Fire::createFileParticle()
{
	// Add particle scene node
	particleSystem = smgr->addParticleSystemSceneNode(false);

	// Grab the pointer, so it won't be destroyed when clearing the scene
	particleSystem->grab();

	// Create emitter
	scene::IParticleEmitter* em = particleSystem->createBoxEmitter(
		core::aabbox3d<f32>(-4, 0, -4, 4, 1, 4),	// Emitter size
		core::vector3df(0.0f, 0.06f, 0.0f),			// Initial direction
		80, 100,									// Emit rate
		video::SColor(0, 255, 255, 255),			// Darkest color
		video::SColor(0, 255, 255, 255),			// Brightest color
		500, 800, 0,								// Min and Max Age, Angle
		core::dimension2df(10.f, 10.f),				// Min size
		core::dimension2df(20.f, 20.f));			// Max size

	particleSystem->setEmitter(em); // This grabs the emitter
	em->drop(); // So we can drop it here without deleting it

	// Fade out effect for particles
	scene::IParticleAffector* paf = particleSystem->createFadeOutParticleAffector();

	particleSystem->addAffector(paf); // Same goes for the affector
	paf->drop();

	// Setup particle system
	particleSystem->setPosition(position + vector3df(0, 15, 0));
	particleSystem->setMaterialFlag(EMF_LIGHTING, false);
	particleSystem->setMaterialFlag(EMF_ZWRITE_ENABLE, false);
	particleSystem->setMaterialFlag(EMF_BLEND_OPERATION, true);
	particleSystem->setMaterialTexture(0, driver->getTexture("textures/particle_fire.png"));
	particleSystem->setMaterialType((E_MATERIAL_TYPE)COMMON_EMT_TRANSPARENT_ADD_COLOR);
}