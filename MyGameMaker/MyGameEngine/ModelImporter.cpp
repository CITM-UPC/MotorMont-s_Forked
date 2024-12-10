#include "ImageImporter.h"
#include "ModelImporter.h"
#include "Mesh.h"
#include "../MyGameEditor/SceneManager.h"
#include <filesystem>
#include <stdexcept>
#include <IL/il.h>
#include <IL/ilu.h>
#include <mutex>

namespace fs = std::filesystem;

// Thread-safe initialization for DevIL
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


// Load image and return shared pointer to Image
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

    // Load image with correct character type
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

    // Convert image format based on number of channels
    if (channels == 1) {
        ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
    }
    else if (channels == 2) {
        ilConvertImage(IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);
    }
    else if (channels == 3) {
        ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
    }
    else if (channels == 4) {
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    }

    const auto* data = ilGetData();

    // Create and populate the Image object
    auto image = std::make_shared<Image>();
    image->load(width, height, channels, (void*)data);

    ilDeleteImages(1, &img);

    return image;
}

GameObject ModelImporter::loadCustomFormat(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open custom model file: " + path);
    }

    // Example parsing logic for custom binary file
    GameObject go;
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    // Read header (example structure)
    uint32_t vertexCount, indexCount;
    file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
    file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

    // Load vertices
    std::vector<glm::vec3> vertices(vertexCount);
    file.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(glm::vec3));

    // Load indices
    std::vector<unsigned int> indices(indexCount);
    file.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(unsigned int));

    // Load into mesh
    mesh->load(vertices.data(), vertices.size(), indices.data(), indices.size());
    go.setMesh(mesh);

    file.close();

    return go;
}

void ModelImporter::saveAsCustomFormat(const GameObject& gameObject, const std::string& outputPath) {
    if (!gameObject.hasMesh()) {
        throw std::runtime_error("GameObject has no mesh to save.");
    }

    const auto& mesh = gameObject.mesh();

    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for saving: " + outputPath);
    }

    // Save vertex count
    uint32_t vertexCount = mesh.vertices().size();
    file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));

    // Save vertices
    file.write(reinterpret_cast<const char*>(mesh.vertices().data()), vertexCount * sizeof(glm::vec3));

    // Save index count
    uint32_t indexCount = mesh.indices().size();
    file.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

    // Save indices
    file.write(reinterpret_cast<const char*>(mesh.indices().data()), indexCount * sizeof(unsigned int));

    file.close();
    std::cout << "Model saved as .custom format: " << outputPath << std::endl;
}
