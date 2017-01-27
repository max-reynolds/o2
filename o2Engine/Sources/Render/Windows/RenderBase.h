#pragma once

#include "Render/Windows/OpenGL.h"

namespace o2
{
	class RenderBase
	{
	protected:
		HGLRC mGLContext; // OpenGL context
		HDC   mHDC;       // Windows frame device context
    };
};
