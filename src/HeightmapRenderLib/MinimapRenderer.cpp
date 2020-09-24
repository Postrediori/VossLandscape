#include "pch.h"
#include "VossHeightmap.h"
#include "Image.h"
#include "IHeightmapRenderer.h"
#include "MinimapRenderer.h"

static const uint32_t WaterColor = 0x007F7FFF;

MinimapRenderer::MinimapRenderer(VossHeightmap* map, Image* image)
    : m_map(map) {
    setImage(image);
}

void MinimapRenderer::draw() {
    for (size_t row = 0; row < m_image->GetHeight(); row++) {
        for (size_t col = 0; col < m_image->GetWidth(); col++) {
            uint32_t color = 0;
            int height = int(double(m_map->GetHeight(col, row)) / m_map->GetSlopeSeek() * 256.);
            if (height < 0) {
                color = WaterColor;
            }
            else {
                color = (height << 16) + (height << 8) + height;
            }
            m_image->putPixel(col, row, color);
        }
    }
}

void MinimapRenderer::setImage(Image* image) {
    assert(image);
    m_image = image;
}
