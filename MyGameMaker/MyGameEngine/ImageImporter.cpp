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

    // Serialize metadata
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(reinterpret_cast<const char*>(&channels), sizeof(channels));

    // Serialize raw image data
    const auto& rawData = image->rawData();
    file.write(reinterpret_cast<const char*>(rawData.data()), rawData.size());

    file.close();
    std::cout << "Image saved as .custom format: " << outputPath << std::endl;
}

std::shared_ptr<Image> ImageImporter::loadCustomImage(const std::string& inputPath) {
    std::ifstream file(inputPath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for loading: " + inputPath);
    }

    uint32_t width, height, channels;

    // Deserialize metadata
    file.read(reinterpret_cast<char*>(&width), sizeof(width));
    file.read(reinterpret_cast<char*>(&height), sizeof(height));
    file.read(reinterpret_cast<char*>(&channels), sizeof(channels));

    if (!file) {
        throw std::runtime_error("Failed to read metadata from: " + inputPath);
    }

    std::vector<unsigned char> rawData(width * height * channels);

    // Deserialize raw image data
    file.read(reinterpret_cast<char*>(rawData.data()), rawData.size());

    if (!file) {
        throw std::runtime_error("Failed to read image data from: " + inputPath);
    }

    auto image = std::make_shared<Image>();
    image->load(width, height, channels, rawData.data());

    file.close();
    std::cout << "Image loaded from .custom format: " << inputPath << std::endl;
    return image;
}