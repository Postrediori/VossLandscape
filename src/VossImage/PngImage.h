#pragma once

#include "BaseImage.h"

class PngImage : public BaseImage {
public:
    PngImage(size_t width, size_t height);

    bool SaveToFile(const std::string& file_name);
};
