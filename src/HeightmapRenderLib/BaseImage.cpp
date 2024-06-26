#include "pch.h"
#include "MathFunctions.h"
#include "BaseImage.h"

static const size_t g_BytesPerPixel = 4;
static const size_t g_BitDepth = 8;

void ConvertColor(uint32_t color, uint8_t rgba[])
{
    rgba[0] = (color >> 16); // Red
    rgba[1] = (color >> 8) & 0xFF; // Green
    rgba[2] = (color & 0xFF); // Blue
    rgba[3] = 0xFF;
}

BaseImage::BaseImage(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
    , m_bitDepth(g_BitDepth)
    , m_bytesPerPixel(g_BytesPerPixel)
{
    m_pitch = clp2(m_width);
    m_data_size = m_pitch * m_height;
    m_data.resize(m_data_size);
    Clear();
}

void BaseImage::Clear()
{
    std::fill(m_data.begin(), m_data.end(), 0);
}

void BaseImage::PutPixel(size_t x, size_t y, uint32_t color)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    m_data[y * m_pitch + x] = color;
}

uint8_t* BaseImage::CreateRGB(size_t* data_size, ImageByteOrder byteOrder)
{
    size_t img_data_size = m_width * m_height * m_bytesPerPixel;

    if (data_size) {
        *data_size = img_data_size;
    }

    uint8_t* data = new uint8_t[img_data_size];

    // Convert 4-byte aligned image data into array of rgb-bytes
    uint8_t* p = data;
    for (size_t row = 0; row < m_height; row++) {
        uint32_t* ofs = (uint32_t *)(m_data.data() + row * m_pitch);

        for (size_t col = 0; col < m_width; col++) {
            if (byteOrder == ImageByteOrder::Reversed) {
                uint8_t rgba[4];
                ConvertColor(*ofs, rgba);
                p[0] = rgba[2];
                p[1] = rgba[1];
                p[2] = rgba[0];
            }
            else if (byteOrder == ImageByteOrder::Direct) {
                ConvertColor(*ofs, p);
            }

            p += m_bytesPerPixel;
            ofs++;
        }
    }

    return data;
}
