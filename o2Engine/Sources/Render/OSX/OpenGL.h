#pragma once

//#define GL_GLEXT_LEGACY
#include <OpenGL/gl3.h>

namespace o2
{
	class LogStream;
}

// Returns opengl error description by id
const char* GetGLErrorDesc(GLenum errorId);

// Checks OpenGL extension supporting
bool IsGLExtensionSupported(const char *extension);

// Checks OpenGL error
void glCheckError(o2::LogStream* log, const char* filename = nullptr, unsigned int line = 0);

#if RENDER_DEBUG
#	define GL_CHECK_ERROR(log) glCheckError(log, __FILE__, __LINE__);
#else
#	define GL_CHECK_ERROR(log) 
#endif
