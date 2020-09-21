#include "stdafx.h"
#include "GlUtils.h"
#include "Shader.h"
#include "ShaderProgram.h"


static const char vertex_src_1_30[] = R"glsl(
#version 330 core
in vec2 coord;
in vec2 tex_coord;
out vec2 xy_coord;
uniform vec2 iRes;
uniform mat4 mvp;
const float aspect_ratio=0.75;
void main(void){
    xy_coord=coord.xy;
    if (iRes.x>iRes.y){
        xy_coord.x*=iRes.y/(iRes.x*aspect_ratio);
    }else{
        xy_coord.y*=iRes.x/(iRes.y*aspect_ratio);
    }
    gl_Position=mvp*vec4(xy_coord,0.,1.);
    xy_coord=tex_coord.xy;
    xy_coord.y=-xy_coord.y;
}
)glsl";

static const char fragment_src_1_30[] = R"glsl(
#version 330 core
in vec2 xy_coord;
out vec4 frag_color;
uniform sampler2D tex;
void main(void){
   frag_color=texture(tex,xy_coord);
   frag_color.a=1.0;
}
)glsl";

/* Orthogonal projection */
static const GLfloat g_OrthoMVP[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};


ShaderProgram::ShaderProgram()
{
    if (Shader::createProgramSource(m_program,
        vertex_src_1_30, fragment_src_1_30)) {
        LOGI << "Using GLSL 1.30 for rendering";
    }

    m_aCoord = glGetAttribLocation(m_program, "coord"); LOGOPENGLERROR();
    m_aTexCoord = glGetAttribLocation(m_program, "tex_coord"); LOGOPENGLERROR();

    m_uMVP = glGetUniformLocation(m_program, "mvp"); LOGOPENGLERROR();
    m_uRes = glGetUniformLocation(m_program, "iRes"); LOGOPENGLERROR();
    m_uTex = glGetUniformLocation(m_program, "tex"); LOGOPENGLERROR();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_program); LOGOPENGLERROR();
}

void ShaderProgram::SetUniforms(int w, int h)
{
    glUniformMatrix4fv(m_uMVP, 1, GL_FALSE, (GLfloat *)(&g_OrthoMVP)); LOGOPENGLERROR();
    glUniform2f(m_uRes, w, h); LOGOPENGLERROR();
    glUniform1i(m_uTex, 0); LOGOPENGLERROR();
}

void ShaderProgram::UseProgram()
{
    glUseProgram(m_program); LOGOPENGLERROR();
}

void ShaderProgram::SetupAttributes()
{
    glEnableVertexAttribArray(m_aCoord); LOGOPENGLERROR();
    glVertexAttribPointer(m_aCoord, 2, GL_FLOAT, GL_FALSE
        , sizeof(GLfloat)*4, 0); LOGOPENGLERROR();

    glEnableVertexAttribArray(m_aTexCoord); LOGOPENGLERROR();
    glVertexAttribPointer(m_aTexCoord, 2, GL_FLOAT, GL_FALSE
        , sizeof(GLfloat)*4, (void *)(sizeof(GLfloat)*2)); LOGOPENGLERROR();
}
