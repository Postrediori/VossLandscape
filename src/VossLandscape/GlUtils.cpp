#include "stdafx.h"
#include "GlUtils.h"

void LogOpenGLError(const char* file, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE << " OpenGL Error in file " << file << " line " << line /*<< " : " << gluErrorString(err)*/;
    }
}

