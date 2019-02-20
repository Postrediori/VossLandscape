#pragma once

class Image;

class TgaImage : public Image
{
public:
    TgaImage(size_t width, size_t height);
    
    bool saveToFile(const std::string& file_name);
};
