#include "pch.h"
#include "BaseHeightmap.h"

BaseHeightmap::BaseHeightmap(size_t nmax)
    : m_width((1 << nmax) + 1)
    , m_width2(m_width * m_width)
{
    m_b.resize(m_width2);
}
