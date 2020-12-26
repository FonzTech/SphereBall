#include "ShaderCallback.h"

void ShaderCallback::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{
	// Get video driver
	video::IVideoDriver* driver = services->getVideoDriver();

	// Set clip matrix
	core::matrix4 mProj = driver->getTransform(video::ETS_PROJECTION);
	core::matrix4 mView = driver->getTransform(video::ETS_VIEW);
	core::matrix4 mWorld = driver->getTransform(video::ETS_WORLD);

	// Pass matrices to shader
	services->setVertexShaderConstant("mWorld", mWorld.pointer(), 16);
	services->setVertexShaderConstant("mView", mView.pointer(), 16);
	services->setVertexShaderConstant("mProj", mProj.pointer(), 16);

	// Set fragment shader constants
	s32 layer0 = 0;
	services->setPixelShaderConstant("tex", &layer0, 1);
}
