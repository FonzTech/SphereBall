#include "GUIImageSceneNode.h"
#include "Utility.h"

#undef snprintf

GUIImageSceneNode::GUIImageSceneNode(scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id) : ISceneNode(parent, smgr, id)
{
	/*
		Here we initialize the vertices of the screen Aligned quad
	*/

	core::dimension2df currentResolution = Utility::getWindowSize<f32>(smgr->getVideoDriver());

	aabb.reset(0, 0, 0);

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

GUIImageSceneNode::~GUIImageSceneNode()
{
}

const core::aabbox3df& GUIImageSceneNode::getBoundingBox() const
{
	return aabb;
}

void GUIImageSceneNode::OnRegisterSceneNode()
{
	/*
		This method is empty because it is best for us to render this scene node manually.
		So, it is never really rendered on its own, if we don't tell it to do so.
	*/
}

void GUIImageSceneNode::ChangeMaterialType(E_MATERIAL_TYPE newMaterial)
{
	material.MaterialType = newMaterial;
}

void GUIImageSceneNode::render()
{
	IVideoDriver* drv = getSceneManager()->getVideoDriver();
	core::matrix4 proj;
	u16 indices[] = { 0, 1, 2, 3, 1, 2 };

	// A triangle list
	drv->setMaterial(material);

	drv->setTransform(ETS_PROJECTION, core::IdentityMatrix);
	drv->setTransform(ETS_VIEW, core::IdentityMatrix);
	drv->setTransform(ETS_WORLD, core::IdentityMatrix);

	drv->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
}

u32 GUIImageSceneNode::getMaterialCount()
{
	// There is only one material
	return 1;
}

SMaterial& GUIImageSceneNode::getMaterial(irr::u32 i)
{
	// We always return the same material, so there is no need for more.
	return material;
}

void GUIImageSceneNode::flipHorizontal()
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

void GUIImageSceneNode::setVertices(const core::vector3df & containerSize, core::vector3df & upperLeft, core::vector3df & upperRight, core::vector3df & lowerLeft, core::vector3df & lowerRight, const core::vector3df* rotationCenter, const core::vector3df* rotationDegrees)
{
	matrix4 rotation;
	if (rotationCenter != nullptr)
	{
		Utility::rotateVectorAroundPivot(upperLeft, upperLeft, *rotationCenter, *rotationDegrees);
		Utility::rotateVectorAroundPivot(upperRight, upperRight, *rotationCenter, *rotationDegrees);
		Utility::rotateVectorAroundPivot(lowerLeft, lowerLeft, *rotationCenter, *rotationDegrees);
		Utility::rotateVectorAroundPivot(lowerRight, lowerRight, *rotationCenter, *rotationDegrees);
	}

	upperLeft = (upperLeft / containerSize * 2.0f - 1.0f) * vector3df(1, -1, 1);
	upperRight = (upperRight / containerSize * 2.0f - 1.0f) * vector3df(1, -1, 1);
	lowerLeft = (lowerLeft / containerSize * 2.0f - 1.0f) * vector3df(1, -1, 1);
	lowerRight = (lowerRight / containerSize * 2.0f - 1.0f) * vector3df(1, -1, 1);

	vertices[0] = S3DVertex2TCoords(upperLeft, vector3df(0.0f, 0.0f, -1.0f), SColor(255, 255, 255, 255), vector2df(0), vector2df(0));
	vertices[1] = S3DVertex2TCoords(upperRight, vector3df(0.0f, 0.0f, -1.0f), SColor(255, 255, 255, 255), vector2df(1, 0), vector2df(1, 0));
	vertices[2] = S3DVertex2TCoords(lowerLeft, vector3df(0.0f, 0.0f, -1.0f), SColor(255, 255, 255, 255), vector2df(0, 1), vector2df(0, 1));
	vertices[3] = S3DVertex2TCoords(lowerRight, vector3df(0.0f, 0.0f, -1.0f), SColor(255, 255, 255, 255), vector2df(1, 1), vector2df(1, 1));
}