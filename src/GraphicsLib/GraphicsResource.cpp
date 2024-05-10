#include "pch.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"

namespace GraphicsResource {

void unique_texture::close() {
    glDeleteTextures(1, &resourceId_); LOGOPENGLERROR();
}

void unique_buffer::close() {
    glDeleteBuffers(1, &resourceId_); LOGOPENGLERROR();
}

void unique_vao::close() {
    glDeleteTextures(1, &resourceId_); LOGOPENGLERROR();
}

void unique_program::close() {
    glDeleteProgram(resourceId_); LOGOPENGLERROR();
}

} // namespace GraphicsResource
