#pragma once

#include "BaseHeightmap.h"
#include "BaseImage.h"

class BaseHeightmapRenderer {
public:
    virtual ~BaseHeightmapRenderer() = default;

    void SetSlopeSeek(double slopeSeek) { m_slopeSeek = slopeSeek; }

    virtual void Draw(BaseImage& image, BaseHeightmap& heightmap) = 0;

protected:
    double m_slopeSeek = 1.0;
};
