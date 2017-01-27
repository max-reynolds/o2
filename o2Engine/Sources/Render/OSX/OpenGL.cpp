#include "OpenGL.h"

#include "Utils/Log/LogStream.h"

bool IsGLExtensionSupported(const char *extension)
{
    return true;
    
	const GLubyte *extensions = NULL;
	const GLubyte *start;

	GLubyte *where, *terminator;
	/* Extension names should not have spaces. */

	where = (GLubyte *)strchr(extension, ' ');

	if (where || *extension == '\0')
		return 0;

	extensions = glGetString(GL_EXTENSIONS);

	/* It takes a bit of care to be fool-proof about parsing the
	OpenGL extensions string. Don't be fooled by sub-strings,
	etc. */

	start = extensions;
	for (;;)
	{
		where = (GLubyte *)strstr((const char *)start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;

		start = terminator;
	}

	return false;
}

const char* GetGLErrorDesc(GLenum errorId)
{
	if (errorId == GL_NO_ERROR) return "GL_NO_ERROR";
	if (errorId == GL_INVALID_ENUM) return "GL_INVALID_ENUM";
	if (errorId == GL_INVALID_VALUE) return "GL_INVALID_VALUE";
	if (errorId == GL_INVALID_OPERATION) return "GL_INVALID_OPERATION";
	if (errorId == GL_INVALID_FRAMEBUFFER_OPERATION) return "GL_INVALID_FRAMEBUFFER_OPERATION";
	if (errorId == GL_OUT_OF_MEMORY) return "GL_OUT_OF_MEMORY";
	//if (errorId == GL_STACK_UNDERFLOW) return "GL_STACK_UNDERFLOW";
	//if (errorId == GL_STACK_OVERFLOW) return "GL_STACK_OVERFLOW";

	return "UNKNOWN";
}

void glCheckError(o2::LogStream* log, const char* filename /*= nullptr*/, unsigned int line /*= 0*/)
{
	GLenum errId = glGetError();
	if (errId != GL_NO_ERROR)
	{
        log->Out((o2::String)"OpenGL ERROR " + (int)errId + ": " + (o2::String)GetGLErrorDesc(errId )+ " at file: " +
                 (o2::String)(filename ? filename : "unknown") + " line: " + (o2::String)line);
	}
}
