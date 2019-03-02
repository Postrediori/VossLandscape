#pragma once

class VertexBuffer
{
public:
    VertexBuffer(GLenum mode, GLsizei count, const GLfloat data[]);
    ~VertexBuffer();

    void BindBuffer();
    void DrawArrays();

private:
    GLenum m_mode;
    GLsizei m_count;
    GLuint m_vbo;
};

