#pragma once

class BaseHeightmap {
public:
    BaseHeightmap(size_t nmax);
    virtual ~BaseHeightmap() = default;

    size_t GetWidth() const { return m_width; }

    int GetHeight(size_t x, size_t y) const { return m_b[y * m_width + x]; }

protected:
    size_t m_width;
    size_t m_width2;
    std::vector<int> m_b;
};
