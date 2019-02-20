#include "stdafx.h"
#include "Math.h"
#include "Image.h"
#include "TgaImage.h"

static const uint8_t TGAHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

TgaImage::TgaImage(size_t width, size_t height)
    : Image(width, height)
{
}

bool TgaImage::saveToFile(const std::string& file_name)
{
    uint8_t header[6];

    size_t bpp = m_bytesPerPixel * 8;

    std::ofstream f(file_name, std::ofstream::binary);
    if (!f) {
        std::cerr << "Cannot open output file " << file_name << std::endl;
        return false;
    }

    f.write(reinterpret_cast<const char *>(TGAHeader), sizeof(TGAHeader));

    header[0] = m_width & 0xFF;  // Lo-bytes of width
    header[1] = m_width >> 8;    // Hi-bytes of width
    header[2] = m_height & 0xFF; // Lo-bytes of height
    header[3] = m_height >> 8;   // Hi-bytes of height
    header[4] = bpp;
    header[5] = 32;             // Image descriptor byte

    f.write(reinterpret_cast<char *>(header), sizeof(header));

    size_t image_data_size;
    std::unique_ptr<uint8_t> image_data(createRGB(&image_data_size, true));

    f.write(reinterpret_cast<char*>(image_data.get()), image_data_size);

    return true;
}
