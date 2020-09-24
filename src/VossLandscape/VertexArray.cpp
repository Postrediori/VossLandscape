#include "pch.h"
#include "GlUtils.h"
#include "ShaderProgram.h"
#include "VertexArray.h"

VertexArray::VertexArray(ShaderProgram& program)
    : m_vao(0)
    , m_program(program)
{
    glGenVertexArrays(1, &m_vao); LOGOPENGLERROR();
    glBindVertexArray(m_vao); LOGOPENGLERROR();

    m_program.SetupAttributes();

    glBindVertexArray(0); LOGOPENGLERROR();
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_vao); LOGOPENGLERROR();
}

void VertexArray::BindVertexArray()
{
    if (m_vao) {
        glBindVertexArray(m_vao); LOGOPENGLERROR();
    }
    else {
        m_program.SetupAttributes();
    }
}

void VertexArray::UnbindVertexArray()
{
    if (m_vao) {
        glBindVertexArray(0); LOGOPENGLERROR();
    }
}
