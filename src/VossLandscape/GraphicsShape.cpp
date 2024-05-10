#include "pch.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"
#include "Shader.h"
#include "GraphicsShape.h"


/*****************************************************************************
 * BaseShape
 ****************************************************************************/

bool BaseShape::Init() {
    glGenVertexArrays(1, &m_vao); LOGOPENGLERROR();
    if (!m_vao) {
        LOGE << "Cannot create VAO";
        return false;
    }
    glBindVertexArray(static_cast<GLuint>(m_vao)); LOGOPENGLERROR();

    glGenBuffers(1, &m_vbo); LOGOPENGLERROR();
    if (!m_vbo) {
        LOGE << "Cannot create VBO";
        return false;
    }

    if (!InitInternal()) {
        return false;
    }

    glBindVertexArray(0); LOGOPENGLERROR();

    return true;
}

void BaseShape::Render() {
    glBindVertexArray(static_cast<GLuint>(m_vao)); LOGOPENGLERROR();

    glDrawArrays(m_drawMode, 0, m_vboCount); LOGOPENGLERROR();

    glBindVertexArray(0); LOGOPENGLERROR();
}


/*****************************************************************************
 * TexturedRectangleShape
 ****************************************************************************/

const char VertexShaderSource[] = R"glsl(#version 330 core
in vec2 coord;
in vec2 tex_coord;
out vec2 xy_coord;
uniform vec2 iRes;
uniform mat4 mvp;
const float aspect_ratio=0.75;
void main() {
    xy_coord=coord.xy;
    vec2 r=iRes*vec2(aspect_ratio,1.);
    xy_coord*=(r.x>r.y)?vec2(r.y/r.x,1.):vec2(1.,r.x/r.y);
    gl_Position=mvp*vec4(xy_coord,0.,1.);
    xy_coord=tex_coord.xy;
    xy_coord.y=-xy_coord.y;
}
)glsl";

const char FragmentShaderSource[] = R"glsl(#version 330 core
in vec2 xy_coord;
out vec4 frag_color;
uniform sampler2D tex;
void main() {
   frag_color=texture(tex,xy_coord);
   frag_color.a=1.0;
}
)glsl";

void TexturedRectangleShape::SetResolution(int w, int h) {
    m_width = w;
    m_height = h;

    glUseProgram((GLuint)m_program); LOGOPENGLERROR();
    glUniform2f(m_uRes, m_width, m_height); LOGOPENGLERROR();
}

bool TexturedRectangleShape::InitInternal() {
    constexpr int QuadVerticesCount = 6;
    const GLfloat QuadVertices[] = {
        -1.f, -1.f,  0.0f, 0.0f,
        1.f, -1.f,  1.0f, 0.0f,
        1.f,  1.f,  1.0f, 1.0f,

        -1.f, -1.f,  0.0f, 0.0f,
         1.f,  1.f,  1.0f, 1.0f,
        -1.f,  1.f,  0.0f, 1.0f,
    };

    m_vboCount = QuadVerticesCount;
    m_drawMode = GL_TRIANGLES;

    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(m_vbo)); LOGOPENGLERROR();
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices,
                GL_STATIC_DRAW); LOGOPENGLERROR();

    m_program.reset(Shader::CreateProgramFromSources(VertexShaderSource, FragmentShaderSource));
    if (!m_program) {
        LOGE << "Cannot create shader program";
        return false;
    }

    // Get uniforms
    m_uMVP = glGetUniformLocation((GLuint)m_program, "mvp"); LOGOPENGLERROR();
    m_uRes = glGetUniformLocation((GLuint)m_program, "iRes"); LOGOPENGLERROR();
    m_uTex = glGetUniformLocation((GLuint)m_program, "tex"); LOGOPENGLERROR();
    if (m_uMVP == -1 || m_uRes == -1 || m_uTex == -1) {
        LOGE << "Cannot get shader uniforms";
        return false;
    }

    // Setup uniforms

    /* Orthogonal projection */
    const GLfloat OrthoMVP[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glUseProgram((GLuint)m_program); LOGOPENGLERROR();

    glUniformMatrix4fv(m_uMVP, 1, GL_FALSE, (GLfloat *)(&OrthoMVP)); LOGOPENGLERROR();
    glUniform1i(m_uTex, 0); LOGOPENGLERROR();

    glUniform2f(m_uRes, m_width, m_height); LOGOPENGLERROR();

    // Setup attributes
    auto aCoord = glGetAttribLocation((GLuint)m_program, "coord"); LOGOPENGLERROR();
    auto aTexCoord = glGetAttribLocation((GLuint)m_program, "tex_coord"); LOGOPENGLERROR();
    if (aCoord == -1 || aTexCoord == -1) {
        LOGE << "Cannot get shader attributes";
        return false;
    }

    glEnableVertexAttribArray(aCoord); LOGOPENGLERROR();
    glVertexAttribPointer(aCoord, 2, GL_FLOAT, GL_FALSE,
        sizeof(GLfloat)*4, reinterpret_cast<void*>(0)); LOGOPENGLERROR();

    glEnableVertexAttribArray(aTexCoord); LOGOPENGLERROR();
    glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE,
        sizeof(GLfloat)*4, reinterpret_cast<void*>(sizeof(GLfloat) * 2)); LOGOPENGLERROR();

    return true;
}
