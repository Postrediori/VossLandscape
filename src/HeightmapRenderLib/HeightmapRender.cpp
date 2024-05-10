#include "pch.h"
#include "MathFunctions.h"
#include "HeightmapRender.h"

constexpr double g_ZScale = 100.0;

constexpr double g_SnowLevel = 50.0;

constexpr uint32_t g_ColorWater = 0x0000AA;
constexpr uint32_t g_ColorEarth = 0xAA5500;
constexpr uint32_t g_ColorSnow  = 0xFFFFFF;

void HeightmapRender::Draw(BaseImage& image, BaseHeightmap& heightmap)
{
    // Set heightmap parameters
    m_xmax = heightmap.GetWidth() - 1; m_xmin = 0.0;
    m_ymax = heightmap.GetWidth() - 1; m_ymin = 0.0;
    m_xh = -1.0;
    m_yh = -1.0;

    // Set image parameters
    m_scale = (image.GetWidth() - 4) * 2.0 /
        (sqrt(3.0) * ((m_xmax - m_xmin) + (m_ymax - m_ymin)));
    m_zScale = g_ZScale;
    m_c.resize(image.GetWidth());

    m_pImage = &image;
    m_pHeightmap = &heightmap;

    // Draw heightmap
    DrawHoriz();
    DrawVert();

    m_pImage = nullptr;
    m_pHeightmap = nullptr;
}

void HeightmapRender::ClearC()
{
    std::fill(m_c.begin(), m_c.end(), 0);
}

void HeightmapRender::DrawHoriz()
{
    ClearC();

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

void HeightmapRender::DrawVert()
{
    ClearC();

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
    m_z = m_pHeightmap->GetHeight(xp, yp) / m_slopeSeek;
    if (m_z <= 0.0) {
        m_z = 0.0;
    }
    m_z *= m_zScale;
    m_xt = (double)(m_pImage->GetWidth() / 2 - 1) + sqrt(3.0) * (m_yi - m_xi) / 2.0;
    m_yt = (double)(m_pImage->GetHeight() - 1) + m_z - (m_yi + m_xi) / 2.0;
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

    uint32_t color = [](double z) {
        if (z <= 0.0) {
            return g_ColorWater;
        }
        else if (z > 0.0 && z <= g_SnowLevel) {
            return g_ColorEarth;
        }
        else {
            return g_ColorSnow;
        }
    }(m_z);

    m_pImage->PutPixel(xp, m_pImage->GetHeight() + 100 - 1 - yp, color);
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
