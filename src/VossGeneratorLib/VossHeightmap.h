#pragma once

class VossHeightmap
{
public:
    VossHeightmap(size_t nmax);
    
    void generate(int x, int y);
    
    size_t GetWidth() const { return m_width; }
    int GetB(size_t index) const { return m_b[index]; }

    double GetSlopeSeek() const { return m_slopeSeek; }

private:
    double drand();
    double nrand(double a, double D);

    void sideVoss(int x1, int y1, int x2, int y2, double D);
    void iterationVoss(int x1, int y1, int x2, int y2, double D);
    
private:
    size_t m_width;
    size_t m_nmax;

    double m_seek;
    double m_heightSeek;
    double m_slopeSeek;

    std::vector<int> m_b;
    std::vector<bool> m_c1;
};
