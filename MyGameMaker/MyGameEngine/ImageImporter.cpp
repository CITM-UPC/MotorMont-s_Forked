#include "ImageImporter.h"
#include <filesystem>
#include <stdexcept>
#include <IL/il.h>
#include <IL/ilu.h>
#include <locale>
#include <codecvt>
#include <mutex>

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

#include <windows.h>
#include <string>

std::wstring toWideString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
    return wstr;
}


std::shared_ptr<Image> ImageImporter::loadFromFile(const std::string& path) {
    initDevIL();

    ILuint img;
    ilGenImages(1, &img);
    ilBindImage(img);

    auto ps = fs::absolute(path);
    if (!fs::exists(ps)) {
        ilDeleteImages(1, &img);
        throw std::runtime_error("File does not exist: " + ps.string());
    }

    // Load image with appropriate string type
    bool image_loaded;
#ifdef UNICODE
    image_loaded = ilLoadImage(toWideString(ps.string()).c_str());
#else
    image_loaded = ilLoadImage(ps.string().c_str());
#endif

    if (!image_loaded) {
        ilDeleteImages(1, &img);
        throw std::runtime_error("Failed to load image: " + ps.string());
    }

    const auto origin = ilGetInteger(IL_IMAGE_ORIGIN);
    if (origin != IL_ORIGIN_UPPER_LEFT) {
        iluFlipImage();
    }

    const auto width = ilGetInteger(IL_IMAGE_WIDTH);
    const auto height = ilGetInteger(IL_IMAGE_HEIGHT);
    const auto channels = ilGetInteger(IL_IMAGE_CHANNELS);

    if (channels == 1) {
        ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
    } else if (channels == 2) {
        ilConvertImage(IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);
    } else if (channels == 3) {
        ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
    } else if (channels == 4) {
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    }

    const auto* data = ilGetData();

    // Create and populate the Image object
    auto image = std::make_shared<Image>();
    image->load(width, height, channels, (void*)data);

    ilDeleteImages(1, &img);

    return image;
}
