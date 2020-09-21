#include "stdafx.h"
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

    int result = stbi_write_png(file_name.c_str(),
        m_width, m_height,
        outputComp,
        m_data.data(),
        outputStride);

    return result;
}
