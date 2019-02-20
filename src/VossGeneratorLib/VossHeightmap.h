#pragma once

extern const double SS;

/*****************************************************************************
 * FossMap
 ****************************************************************************/
class VossHeightmap
{
public:
    VossHeightmap(size_t nmax);
    
    void generate(int x, int y);
    
    size_t GetWidth() const { return m_width; }
    int GetB(size_t index) const { return m_b[index]; }
    
private:
    size_t m_width;
    std::vector<int> m_b;
    
private:
    double m_seek;
    size_t m_nmax;
    std::vector<bool> m_c1;

    double drand();
    double nrand(double a, double D);
    void fossSide(int x1, int y1, int x2, int y2, double D);
    void iterationFoss(int x1, int y1, int x2, int y2, double D);
};
