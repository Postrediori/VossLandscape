#pragma once

class GlImage : public Image
{
public:
    GlImage(size_t width, size_t height);
    ~GlImage();
    
    void CopyToTexture();

    void BindTexture();

private:
    bool InitTexture();

private:
    GLuint m_texture;
};

