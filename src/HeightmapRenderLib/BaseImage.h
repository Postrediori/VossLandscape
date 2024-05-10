#pragma once

enum class ImageByteOrder : uint8_t
{
    Direct,
    Reversed
};

class BaseImage
{
public:
    BaseImage(size_t width, size_t height);
    virtual ~BaseImage() = default;

    void Clear();
    virtual bool SaveToFile(const std::string& /*file_name*/) = 0;

    size_t GetWidth() const { return m_width; }
    size_t GetHeight() const { return m_height; };

    void PutPixel(size_t x, size_t y, uint32_t color);

protected:
    uint8_t* CreateRGB(size_t* data_size, ImageByteOrder byteOrder);

protected:
    size_t m_width;
    size_t m_height;

    size_t m_pitch;
    std::vector<uint32_t> m_data;
    size_t m_data_size;

    size_t m_bitDepth;
    size_t m_bytesPerPixel;
};
