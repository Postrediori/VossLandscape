#include "pch.h"
#include "MinimapRenderer.h"

constexpr uint32_t WaterColor = 0x007F7FFF;

void MinimapRenderer::Draw(BaseImage& image, BaseHeightmap& heightmap) {
    int max = 0;
    for (size_t row = 0; row < image.GetHeight(); row++) {
        for (size_t col = 0; col < image.GetWidth(); col++) {
            auto height = heightmap.GetHeight(col, row);
            if (max < height) {
                max = height;
            }
        }
    }

    for (size_t row = 0; row < image.GetHeight(); row++) {
        for (size_t col = 0; col < image.GetWidth(); col++) {
            auto height = (int)((double)(heightmap.GetHeight(heightmap.GetWidth() - col - 1, row)) / max * 256.);

            uint32_t color = [](int h) {
                if (h < 0) {
                    return WaterColor;
                }
                else {
                    return (uint32_t)((h & 0xFF) << 16) + ((h & 0xFF) << 8) + (h & 0xFF);
                }
            }(height);

            image.PutPixel(col, row, color);
        }
    }
}
