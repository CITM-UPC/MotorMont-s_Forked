#pragma once

#include <string>
#include <memory>
#include "Image.h"

class ImageImporter {
public:
    static std::shared_ptr<Image> loadFromFile(const std::string& path);
    static void saveAsCustomImage(const std::shared_ptr<Image>& image, const std::string& outputPath);
    static std::shared_ptr<Image> loadCustomImage(const std::string& inputPath); // Added function
};
