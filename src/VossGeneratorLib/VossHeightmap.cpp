#include "pch.h"
#include "MathFunctions.h"
#include "BaseHeightmap.h"
#include "VossHeightmap.h"

constexpr double AS = 0.0;
constexpr double BS = 0.0;
constexpr double CS = 0.0;
constexpr double DS = 0.0;
constexpr double g_HeightSeek = 2.0;
constexpr double m_SlopeSeek = 2200.0;

#define b(x,y) m_b[(y)*m_width+(x)]
#define c1(x,y) m_c1[(y)*m_width+(x)]

VossHeightmap::VossHeightmap(size_t nmax)
    : BaseHeightmap(nmax)
    , m_seek(0.0)
    , m_heightSeek(g_HeightSeek)
    , m_slopeSeek(m_SlopeSeek)
{
    m_c1.resize(m_width2);
}

void VossHeightmap::Generate(int x, int y)
{
    std::fill(m_c1.begin(), m_c1.end(), true);
    std::fill(m_b.begin(), m_b.end(), 0);

    m_b[0] = AS; // Upper-left
    m_b[m_width - 1] = BS; // Upper-right
    m_b[m_width * (m_width - 1) - 1] = CS; // Lower-left
    m_b[m_width * m_width - 1] = DS; // Lower-right

    m_seek = x * 1e-3 + y * 1e-6;
    SideVoss(0, 0, m_width - 1, 0, 1.0);
    m_seek = y * 1e-3 + x * 1e-6 + 0.1;
    SideVoss(0, 0, 0, m_width - 1, 1.0);

    m_seek = (x + 1) * 1e-3 + y * 1e-6;
    SideVoss(0, m_width - 1, m_width - 1, m_width - 1, 1.0);
    m_seek = (y + 1) * 1e-3 + x * 1e-6 + 0.1;
    SideVoss(m_width - 1, 0, m_width - 1, m_width - 1, 1.0);

    IterationVoss(0, 0, m_width - 1, m_width - 1, 1.0);
}

void VossHeightmap::SideVoss(int x1, int y1, int x2, int y2, double D)
{
    if (x1 == x2) {
        if ((y2 - y1) <= 1) {
            return;
        }
        int y3 = y1 + (y2 - y1) / 2;

        b(x1, y3) = ((b(x1, y1) + b(x1, y2)) / 2) +
                    round(m_slopeSeek * nrand(0.0, D));
        c1(x1, y3) = false;

        D = exp(2.0 * m_heightSeek * log(0.5)) * D;
        SideVoss(x1, y1, x2, y3, D);
        SideVoss(x1, y3, x2, y2, D);
    }
    else {
        if ((x2 - x1) <= 1) {
            return;
        }
        int x3 = x1 + (x2 - x1) / 2;

        b(x3, y1) = ((b(x1, y1) + b(x2, y1)) / 2) +
                    round(m_slopeSeek * nrand(0.0, D));
        c1(x3, y1) = false;

        D = exp(2.0 * m_heightSeek * log(0.5)) * D;
        SideVoss(x1, y1, x3, y2, D);
        SideVoss(x3, y1, x2, y2, D);
    }
}

void VossHeightmap::IterationVoss(int x1, int y1, int x2, int y2, double D)
{
    if (((x2 - x1) <= 1)
        || ((y2 - y1) <= 1)) {
        return;
    }

    int x3 = x1 + (x2 - x1) / 2;
    int y3 = y1 + (y2 - y1) / 2;

    if (c1(x3, y3)) {
        b(x3, y3) = ((b(x1, y1) + b(x1, y2) + b(x2, y1) + b(x2, y2)) / 4)
            + round(m_slopeSeek * nrand(0.0, D));
        c1(x3, y3) = false;
    }

    if (c1(x1, y3)) {
        b(x1, y3) = ((b(x1, y1) + b(x1, y2)) / 2)
            + round(m_slopeSeek * nrand(0.0, D));
        c1(x1, y3) = false;
    }

    if (c1(x3, y1)) {
        b(x3, y1) = ((b(x1, y1) + b(x2, y1)) / 2)
            + round(m_slopeSeek * nrand(0.0, D));
        c1(x3, y1) = false;
    }

    if (c1(x2, y3)) {
        b(x2, y3) = ((b(x2, y1) + b(x2, y2)) / 2)
            + round(m_slopeSeek * nrand(0.0, D));
        c1(x2, y3) = false;
    }

    if (c1(x3, y2)) {
        b(x3, y2) = ((b(x1, y2) + b(x2, y2)) / 2)
            + round(m_slopeSeek * nrand(0.0, D));
        c1(x3, y2) = false;
    }

    D = exp(1.0 * m_heightSeek * log(0.5)) * D;

    IterationVoss(x1, y1, x3, y3, D);
    IterationVoss(x3, y1, x2, y3, D);
    IterationVoss(x3, y3, x2, y2, D);
    IterationVoss(x1, y3, x3, y2, D);
}

double VossHeightmap::drand()
{
    m_seek = frac(11.0 * m_seek + M_PI);
    return m_seek;
}

double VossHeightmap::nrand(double a, double D)
{
    double x = 0.0;
    for (int i = 0; i < 10; i++) {
        x += drand();
    }
    x = (x - 5.0) * sqrt(D) / sqrt(10.0 / 12.0) + a;
    return x;
}

void VossHeightmap::SetSeekValues(double heightSeek, double slopeSeek)
{
    m_heightSeek = heightSeek;
    m_slopeSeek = slopeSeek;
}
