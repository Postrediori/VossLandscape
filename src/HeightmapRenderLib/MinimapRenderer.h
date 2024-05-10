#pragma once

#include "BaseImage.h"
#include "BaseHeightmapRenderer.h"

class MinimapRenderer : public BaseHeightmapRenderer {
public:
    void Draw(BaseImage& image, BaseHeightmap& heightmap) override;
};
