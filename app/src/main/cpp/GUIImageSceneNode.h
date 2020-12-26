#ifndef GUIIMAGESCENENODE_H
#define GUIIMAGESCENENODE_H

#include <irrlicht.h>

using namespace irr;
using namespace video;

class GUIImageSceneNode : public scene::ISceneNode
{
protected:

	// An axis aligned bounding box. Actually not needed.
	core::aabbox3df aabb;

	// The material used to render the Scene Node
	SMaterial material;

	/*
		The vertices of the Scene Node.
		Normally we wouldn't need more than one set of UV coordinates.
		But if we are to use the builtin materials, this is necessary.
	*/
	S3DVertex2TCoords vertices[4];

	core::vector3df rotationCenter;
	core::vector3df rotationDegrees;

public:
	GUIImageSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id);
	~GUIImageSceneNode();

	const core::aabbox3df& getBoundingBox() const;

	void OnRegisterSceneNode();
	void ChangeMaterialType(E_MATERIAL_TYPE newMaterial);
	void render();
	u32 getMaterialCount();
	SMaterial& getMaterial(u32 i);
	void flipHorizontal();

	void setVertices(const core::vector3df & containerSize, core::vector3df & upperLeft, core::vector3df & upperRight, core::vector3df & lowerLeft, core::vector3df & lowerRight, const core::vector3df* rotationCenter = nullptr, const core::vector3df* rotationDegrees = nullptr);
};

#endif // GUIIMAGESCENENODE_H