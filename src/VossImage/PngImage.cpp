#include "pch.h"
#include "MathFunctions.h"
#include "Image.h"
#include "PngImage.h"

PngImage::PngImage(size_t width, size_t height)
    : Image(width, height)
{
}

bool PngImage::saveToFile(const std::string& file_name)
{
    int outputComp = m_bytesPerPixel;
    int outputStride = m_width * m_bytesPerPixel;

    std::unique_ptr < uint8_t, std::function<void(uint8_t*)>> image_data(
        createRGB(nullptr, ImageByteOrder::Direct),
        [](uint8_t* p) { delete[] p; });

    int result = stbi_write_png(file_name.c_str(),
        m_width, m_height,
        outputComp,
        image_data.get(),
        outputStride);

    return result;
}
