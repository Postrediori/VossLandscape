#include "stdafx.h"
#include "GlUtils.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(GLenum mode, GLsizei count, const GLfloat data[])
    : m_mode(mode)
    , m_count(count)
{
    GLuint genbuf[1];
    glGenBuffers(1, genbuf); LOGOPENGLERROR();
    m_vbo = genbuf[0];
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo); LOGOPENGLERROR();
    glBufferData(GL_ARRAY_BUFFER, sizeof(data[0]) * m_count * 4
        , data, GL_STATIC_DRAW); LOGOPENGLERROR();
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_vbo); LOGOPENGLERROR(); 
}

void VertexBuffer::BindBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo); LOGOPENGLERROR();
}

void VertexBuffer::DrawArrays()
{
    glDrawArrays(m_mode, 0, m_count); LOGOPENGLERROR();
}

