#pragma once

#include "BaseHeightmap.h"

class VossHeightmap : public BaseHeightmap {
public:
    VossHeightmap(size_t nmax);

    void Generate(int x, int y);

    void SetSeekValues(double heightSeek, double slopeSeek);

    double GetSlopeSeek() const { return m_slopeSeek; }

private:
    double drand();
    double nrand(double a, double D);

    void SideVoss(int x1, int y1, int x2, int y2, double D);
    void IterationVoss(int x1, int y1, int x2, int y2, double D);

private:
    double m_seek;
    double m_heightSeek;
    double m_slopeSeek;

    std::vector<bool> m_c1;
};
