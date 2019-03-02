#pragma once

class VertexArray
{
public:
    VertexArray(ShaderProgram& program);
    ~VertexArray();

    void BindVertexArray();
    void UnbindVertexArray();

private:
    GLuint m_vao;
    ShaderProgram& m_program;
};

