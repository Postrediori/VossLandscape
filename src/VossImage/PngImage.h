#pragma once

class Image;

class PngImage : public Image
{
public:
    PngImage(size_t width, size_t height);
    
    bool saveToFile(const std::string& file_name);
};
