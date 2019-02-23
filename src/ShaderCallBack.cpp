#include "ShaderCallback.h"

void ShaderCallBack::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{
	// Get video driver
	video::IVideoDriver* driver = services->getVideoDriver();

	// Set clip matrix
	core::matrix4 worldViewProj;
	worldViewProj = driver->getTransform(video::ETS_PROJECTION);
	worldViewProj *= driver->getTransform(video::ETS_VIEW);
	worldViewProj *= driver->getTransform(video::ETS_WORLD);

	// Set View-Projection matrix
	services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
}