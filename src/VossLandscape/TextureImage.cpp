#include "pch.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"
#include "TextureImage.h"

TextureImage::TextureImage(size_t width, size_t height)
    : BaseImage(width, height)
{
    InitTexture();
}

bool TextureImage::InitTexture()
{
    // Init textures
    glGenTextures(1, m_texture.put()); LOGOPENGLERROR();
    if (!m_texture) {
        LOGE << "Unable to initialize texture";
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, (GLuint)m_texture); LOGOPENGLERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); LOGOPENGLERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); LOGOPENGLERROR();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr); LOGOPENGLERROR();

    return true;
}

void TextureImage::CopyToTexture()
{
    std::unique_ptr<uint8_t[]> image_data(CreateRGB(nullptr, ImageByteOrder::Direct));

    glBindTexture(GL_TEXTURE_2D, (GLuint)m_texture); LOGOPENGLERROR();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA,
        GL_UNSIGNED_BYTE, reinterpret_cast<const GLubyte *>(image_data.get())); LOGOPENGLERROR();
}

void TextureImage::BindTexture()
{
    glActiveTexture(GL_TEXTURE0); LOGOPENGLERROR();
    glBindTexture(GL_TEXTURE_2D, (GLuint)m_texture); LOGOPENGLERROR();
}
