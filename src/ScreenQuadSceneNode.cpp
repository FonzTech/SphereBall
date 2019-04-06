#include "ScreenQuadSceneNode.h"
#include "Utility.h"

ScreenQuadSceneNode::ScreenQuadSceneNode(scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id) : ISceneNode(parent, smgr, id)
{
	/*
		Here we initialize the vertices of the screen Aligned quad
	*/

	core::dimension2df currentResolution = utility::getWindowSize<f32>(smgr->getVideoDriver());

	aabb.reset(0, 0, 0);

	vertices[0] = S3DVertex2TCoords(vector3df(-1.0f, -1.0f, 0.0f), vector3df(0.0f, 0.0f, -1.0f), SColor(255, 255, 255, 255), vector2df(0), vector2df(0));
	vertices[1] = S3DVertex2TCoords(vector3df(1.0f, -1.0, 0.0f), vector3df(0.0f, 0.0f, -1.0f), SColor(255, 255, 255, 255), vector2df(1, 0), vector2df(1, 0));
	vertices[2] = S3DVertex2TCoords(vector3df(-1.0f, 1.0, 0.0f), vector3df(0.0f, 0.0f, -1.0f), SColor(255, 255, 255, 255), vector2df(0, 1), vector2df(0, 1));
	vertices[3] = S3DVertex2TCoords(vector3df(1.0f, 1.0f, 0.0f), vector3df(0.0f, 0.0f, -1.0f), SColor(255, 255, 255, 255), vector2df(1, 1), vector2df(1, 1));

	/*
		Now we proceed to initialize the appropriate settings for the material we are going to use
		We can alter these later, but for the time being, initializing them here will do no harm.
	*/

	// No need for lighting.
	material.Lighting = false;

	// This will add both first and second textures
	material.MaterialType = EMT_SOLID;

	// Not needed, but simplifies things
	material.BackfaceCulling = false;

	// We don't need this scene node to be culled because we render it in screen space.
	setAutomaticCulling(scene::EAC_OFF);

	// Enable Z-Buffering
	material.ZBuffer = ECFN_ALWAYS;
	material.ZWriteEnable = false;
}

ScreenQuadSceneNode::~ScreenQuadSceneNode()
{
}

const core::aabbox3df& ScreenQuadSceneNode::getBoundingBox() const
{
	return aabb;
}

void ScreenQuadSceneNode::OnRegisterSceneNode()
{
	/*
		This method is empty because it is best for us to render this scene node manually.
		So, it is never really rendered on its own, if we don't tell it to do so.
	*/
}

void ScreenQuadSceneNode::ChangeMaterialType(E_MATERIAL_TYPE newMaterial)
{
	material.MaterialType = newMaterial;
}

void ScreenQuadSceneNode::render()
{
	IVideoDriver* drv = getSceneManager()->getVideoDriver();
	core::matrix4 proj;
	u16 indices[] = { 0,1,2,3,1,2 };

	// A triangle list

	drv->setMaterial(material);

	drv->setTransform(ETS_PROJECTION, core::IdentityMatrix);
	drv->setTransform(ETS_VIEW, core::IdentityMatrix);
	drv->setTransform(ETS_WORLD, core::IdentityMatrix);

	drv->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
}

u32 ScreenQuadSceneNode::getMaterialCount()
{
	// There is only one material
	return 1;
}

SMaterial& ScreenQuadSceneNode::getMaterial(irr::u32 i)
{
	// We always return the same material, so there is no need for more.
	return material;
}

void ScreenQuadSceneNode::flipHorizontal()
{
	core::vector2d<f32> temp;
	temp = vertices[2].TCoords;
	vertices[2].TCoords = vertices[0].TCoords;
	vertices[0].TCoords = temp;

	temp = vertices[2].TCoords2;
	vertices[2].TCoords2 = vertices[0].TCoords2;
	vertices[0].TCoords2 = temp;

	temp = vertices[3].TCoords;
	vertices[3].TCoords = vertices[1].TCoords;
	vertices[1].TCoords = temp;

	temp = vertices[3].TCoords2;
	vertices[3].TCoords2 = vertices[1].TCoords2;
	vertices[1].TCoords2 = temp;
}