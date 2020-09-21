#pragma once

class GlImage : public Image
{
public:
    GlImage(size_t width, size_t height);
    ~GlImage();
    
    void CopyToTexture();

    void BindTexture();
    
    GLuint GetTexture() const { return m_texture; }

private:
    bool InitTexture();

private:
    GLuint m_texture;
};

