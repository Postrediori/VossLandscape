#include "stdafx.h"
#include "Math.h"
#include "Image.h"
#include "PngImage.h"

PngImage::PngImage(size_t width, size_t height)
    : Image(width, height)
{
}

bool PngImage::saveToFile(const std::string& file_name)
{
    std::unique_ptr<FILE, int(*)(FILE*)> f(fopen(file_name.c_str(), "wb"), fclose);
    if (!f) {
        std::cerr << "Cannot open output file " << file_name << std::endl;
        return false;
    }
    
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, f.get());
    
    // Write PNG header
    png_set_IHDR(png_ptr, info_ptr, m_width, m_height, m_bitDepth, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);
    
    png_write_info(png_ptr, info_ptr);
    
    // Create RGB array
    std::unique_ptr<uint8_t> image_data(createRGB());
    
    // Write bytes
    std::vector<png_bytep> row_pointers(m_height);
    for (size_t row = 0; row < m_height; row++) {
        row_pointers[row] = (png_bytep)(image_data.get() + row * m_width * m_bytesPerPixel);
    }
    
    png_write_image(png_ptr, row_pointers.data());
    png_write_end(png_ptr, NULL);
    
    // Clean up
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    return true;
}
