#pragma once

#include "BaseImage.h"

class TextureImage : public BaseImage
{
public:
    TextureImage(size_t width, size_t height);

    void CopyToTexture();

    void BindTexture();

    GLuint GetTexture() const { return (GLuint)m_texture; }

    bool SaveToFile(const std::string& /*file_name*/) override { return false; }

private:
    bool InitTexture();

private:
    GraphicsResource::unique_texture m_texture;
};
