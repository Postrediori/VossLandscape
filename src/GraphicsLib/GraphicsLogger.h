#pragma once

#ifdef NDEBUG
# define LOGOPENGLERROR()
#else
# define LOGOPENGLERROR() GraphicsUtils::LogOpenGLError(__FILE__,__LINE__)
#endif

namespace GraphicsUtils
{
    void LogOpenGLError(const char* file, int line);
}
