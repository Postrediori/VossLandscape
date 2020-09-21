#include "stdafx.h"
#include "MathFunctions.h"
#include "VossHeightmap.h"
#include "Image.h"
#include "IHeightmapRenderer.h"
#include "HeightmapRender.h"

static const double g_ZScale = 100.0;

static const double g_SnowLevel = 50.0;

static const uint32_t g_ColorWater = 0x0000AA;
static const uint32_t g_ColorEarth = 0xAA5500;
static const uint32_t g_ColorSnow  = 0xFFFFFF;

HeightmapRender::HeightmapRender(VossHeightmap* map, Image* image)
    : m_map(map)
{
    m_xmax = map->GetWidth() - 1; m_xmin = 0.0;
    m_ymax = map->GetWidth() - 1; m_ymin = 0.0;
    m_xh = -1.0;
    m_yh = -1.0;

    setImage(image);
}

void HeightmapRender::draw()
{
    drawHoriz();
    drawVert();
}

void HeightmapRender::setImage(Image* image)
{
    assert(image);
    m_image = image;

    m_scale = (m_image->GetWidth() - 4) * 2.0 /
        (sqrt(3.0) * ((m_xmax - m_xmin) + (m_ymax - m_ymin)));
    m_zScale = g_ZScale;
    m_c.resize(m_image->GetWidth());
}

void HeightmapRender::clearC()
{
    std::fill(m_c.begin(), m_c.end(), 0);
}

void HeightmapRender::drawHoriz()
{
    clearC();

    m_y = m_ymax;
    while (m_y >= m_ymin) {
        m_yi = m_y * m_scale;

        m_x = m_xmax;
        while (m_x >= m_xmin) {
            m_xi = m_x * m_scale;
            p5000();
            if (m_x != m_xmax) {
                m_dx = m_xt - m_xold;
                m_dy = m_yt - m_yold;
                if (fabs(m_dy) >= fabs(m_dx)) {
                    p6000();
                }
                else {
                    p7000();
                }
            }
            else {
                p8100();
            }
            m_x += m_xh;
        }
        m_y += m_yh;
    }
}

void HeightmapRender::drawVert()
{
    clearC();

    m_x = m_xmax;
    while (m_x >= m_xmin) {
        m_xi = m_x * m_scale;

        m_y = m_ymax;
        while (m_y >= m_ymin) {
            m_yi = m_y * m_scale;
            p5000();
            if (m_y != m_ymax) {
                m_dx = m_xt - m_xold;
                m_dy = m_yt - m_yold;
                if (fabs(m_dy) > fabs(m_dx)) {
                    p6000();
                }
                else {
                    p7000();
                }
            }
            else {
                p8100();
            }
            m_y += m_yh;
        }
        m_x += m_xh;
    }
}

void HeightmapRender::p5000()
{
    int xp = iround(m_x);
    int yp = iround(m_y);
    m_z = m_map->GetHeight(xp, yp) / m_map->GetSlopeSeek();
    if (m_z <= 0.0) {
        m_z = 0.0;
    }
    m_z *= m_zScale;
    m_xt = double(m_image->GetWidth() / 2 - 1) + sqrt(3.0) * (m_yi - m_xi) / 2.0;
    m_yt = double(m_image->GetHeight() - 1) + m_z - (m_yi + m_xi) / 2.0;
}

void HeightmapRender::p6000() {
    if (m_dy < 0.0) {
        for (int i = iround(m_dy); i <= 0; i++) {
            p6500(i);
        }
        p8200();
    }
    else {
        for (int i = 0; i <= iround(m_dy); i++) {
            p6500(i);
        }
        p8100();
    }
}

void HeightmapRender::p6500(int tick)
{
    m_xt = m_xold + m_dx / m_dy * tick;
    m_yt = m_yold + tick;
    p8000();
}

void HeightmapRender::p7000()
{
    if (m_dx < 0.0) {
        for (int i = iround(m_dx); i <= 0; i++) {
            p7500(i);
        }
        p8200();
        
    }
    else {
        for (int i = 0; i <= iround(m_dx); i++) {
            p7500(i);
        }
        p8100();
    }
}

void HeightmapRender::p7500(int tick)
{
    m_xt = m_xold + tick;
    m_yt = m_yold + m_dy / m_dx * tick;
    p8000();
}

void HeightmapRender::p8000()
{
    int xp = iround(m_xt);
    int yp = iround(m_yt);
    
    if (m_yt <= m_c[xp]) {
        return;
    }
    
    m_c[xp] = yp;

    uint32_t color;
    if (m_z <= 0.0) {
        color = g_ColorWater;
    }
    else if (m_z > 0.0 && m_z <= g_SnowLevel) {
        color = g_ColorEarth;
    }
    else {
        color = g_ColorSnow;
    }

    m_image->putPixel(xp, m_image->GetHeight() + 100 - 1 - yp, color);
}

void HeightmapRender::p8100()
{
    m_xold = m_xt;
    m_yold = m_yt;
}

void HeightmapRender::p8200()
{
    m_xold += m_dx;
    m_yold += m_dy;
}
