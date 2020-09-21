#pragma once

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    void SetUniforms(int w, int h);

    void UseProgram();

    void SetupAttributes();

private:
    GLuint m_program;

    GLint m_aCoord, m_aTexCoord;
    GLint m_uMVP, m_uRes, m_uTex;
};
