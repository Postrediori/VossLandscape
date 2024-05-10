#pragma once


/*****************************************************************************
 * BaseShape
 ****************************************************************************/

class BaseShape {
public:
    ~BaseShape() = default;

    bool Init();

    void Render();

private:
    virtual bool InitInternal() = 0;

protected:
    GraphicsResource::unique_vao m_vao;
    GraphicsResource::unique_buffer m_vbo;
    GLenum m_drawMode = GL_POINTS;
    GLsizei m_vboCount = 0;
};


/*****************************************************************************
 * TexturedRectangleShape
 ****************************************************************************/

class TexturedRectangleShape : public BaseShape {
public:
    void SetResolution(int width, int height);

private:
    bool InitInternal() override;

private:
    int m_width = 1, m_height = 1;
    GraphicsResource::unique_program m_program;
    GLint m_uMVP, m_uRes, m_uTex;
};
