#include "ImageImporter.h"
#include <filesystem>
#include <stdexcept>
#include <IL/il.h>
#include <IL/ilu.h>
#include <locale>
#include <codecvt>
#include <mutex>
#include <fstream>
#include <iostream>
#include "Image.h"
#include <windows.h>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static void initDevIL() {
    static bool ilInitialized = false;
    static std::mutex ilMutex;
    std::lock_guard<std::mutex> lock(ilMutex);
    if (!ilInitialized) {
        ilInit();
        iluInit();
        ilInitialized = true;
    }
}

// Save the image data by serializing it into a vector
void ImageImporter::saveAsCustomImage(const std::shared_ptr<Image>& image, const std::string& outputPath) {
    if (!image) {
        throw std::runtime_error("No image to save.");
    }

    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for saving: " + outputPath);
    }

    uint32_t width = image->width();
    uint32_t height = image->height();
    uint32_t channels = image->channels();

    // Serialize image metadata
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(reinterpret_cast<const char*>(&channels), sizeof(channels));

    // Serialize image pixel data using rawData()
    const auto& rawData = image->rawData();
    file.write(reinterpret_cast<const char*>(rawData.data()), rawData.size());

    file.close();
    std::cout << "Image saved as .customimage format: " << outputPath << std::endl;
}