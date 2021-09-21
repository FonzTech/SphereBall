#ifndef SHADERCALLBACK_H
#define SHADERCALLBACK_H

#include <irrlicht.h>

using namespace irr;

class ShaderCallback : public video::IShaderConstantSetCallBack
{
public:

	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData);
};

#endif // SHADERCALLBACK_H
