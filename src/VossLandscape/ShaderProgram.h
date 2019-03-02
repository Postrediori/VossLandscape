#pragma once

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    int GetShaderVer() const { return m_shaderVer; }

    void SetUniforms(int w, int h);

    void UseProgram();

    void SetupAttributes();

private:
    int m_shaderVer;

    GLuint m_program;
    GLuint m_vertex, m_fragment;

    GLint m_aCoord, m_aTexCoord;
    GLint m_uMVP, m_uRes, m_uTex;
};
