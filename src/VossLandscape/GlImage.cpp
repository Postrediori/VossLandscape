#include "stdafx.h"
#include "GlUtils.h"
#include "Image.h"
#include "GlImage.h"

GlImage::GlImage(size_t width, size_t height)
    : Image(width, height)
{
    InitTexture();
}

GlImage::~GlImage()
{
    glDeleteTextures(1, &m_texture); LOGOPENGLERROR();
}

bool GlImage::InitTexture()
{
    GLuint genbuf[1];

    // Init textures
    glGenTextures(1, genbuf); LOGOPENGLERROR();
    m_texture = genbuf[0];
    if (!m_texture) {
        LOGE << "Unable to initialize texture for Amari Renderer";
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, m_texture); LOGOPENGLERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); LOGOPENGLERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); LOGOPENGLERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL); LOGOPENGLERROR();

    return true;
}

void GlImage::CopyToTexture()
{
    std::unique_ptr<uint8_t> image_data(createRGB());

    glBindTexture(GL_TEXTURE_2D, m_texture); LOGOPENGLERROR();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB,
        GL_UNSIGNED_BYTE, (const GLubyte *)image_data.get()); LOGOPENGLERROR();
}

void GlImage::BindTexture()
{
    glActiveTexture(GL_TEXTURE0); LOGOPENGLERROR();
    glBindTexture(GL_TEXTURE_2D, m_texture); LOGOPENGLERROR();
}
