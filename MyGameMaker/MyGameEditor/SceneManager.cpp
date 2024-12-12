#include "SceneManager.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Image.h"
#include "Console.h"
#include "MyGameEngine/ModelImporter.h"
#include "MyGameEngine/ImageImporter.h"
#include "MyGameEngine/Texture.h"
#include "BasicShapesManager.h"

#include "MyGameEngine/CameraComponent.h"
#include "MyGameEngine/Camera.h"
#include "MyGameEngine/TransformComponent.h"

#include "SceneManager.h"
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include "MyGui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;


std::vector<GameObject> SceneManager::gameObjectsOnScene;
std::vector<GameObject> SceneManager::savedSceneState;

std::vector<Transform> SceneManager::initialTransforms;

GameObject* SceneManager::selectedObject = nullptr;
bool SceneManager::isPlaying = false;


void SceneManager::restoreSceneState(const std::string& filePath) {
    loadScene(filePath);
    Console::Instance().Log("Scene state restored from " + filePath);
}

void SceneManager::saveSceneState(const std::string& filePath) {
    saveScene(filePath);
    Console::Instance().Log("Scene state saved to " + filePath);
}

void SceneManager::startPlayback() {
    saveSceneState("temp_scene_state.scene");
    isPlaying = true;
    Console::Instance().Log("Playback started.");
}

void SceneManager::stopPlayback() {
    restoreSceneState("temp_scene_state.scene");
    isPlaying = false;
    Console::Instance().Log("Playback stopped.");
}

std::string SceneManager::getFileDirectory(const std::string& filePath) {
    return std::filesystem::path(filePath).parent_path().string();
}

std::string SceneManager::getFileNameWithoutExtension(const std::string& filePath) {
    return std::filesystem::path(filePath).stem().string();
}

bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

void SceneManager::spawnBakerHouse()
{
    GameObject go;
    auto mesh = std::make_shared<Mesh>();
    mesh->LoadFile("Assets/BakerHouse.fbx");
    go.setMesh(mesh);
    auto imageTexture = std::make_shared<Image>();
    imageTexture->loadTexture("Assets/Baker_House.png");
    go.setTextureImage(imageTexture);

    // Asegúrate de obtener el componente TransformComponent y luego acceder a Transform
    auto transformComponent = go.GetComponent<TransformComponent>();
    if (transformComponent) {
        transformComponent->transform().translate(glm::vec3(0, 0, 7));
    }

    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    SceneManager::gameObjectsOnScene.push_back(go);
}



void SceneManager::LoadGameObject(const std::string& filePath) {
    auto mesh = std::make_shared<Mesh>();
    GameObject go;
    mesh->LoadFile(filePath.c_str());
    go.setMesh(mesh);

    // Check for associated texture
    std::string texturePath = filePath.substr(0, filePath.find_last_of('.')) + ".png";
    if (fileExists(texturePath)) {
        auto texture = ImageImporter::loadFromFile(texturePath);
        go.setTextureImage(texture);

        // Save the texture only if it exists
        std::string customImagePath = filePath.substr(0, filePath.find_last_of('.')) + ".customimage";
        ImageImporter::saveAsCustomImage(texture, customImagePath);
    }

    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    gameObjectsOnScene.push_back(go);

    // Save the model in custom format
    std::string customFilePath = filePath.substr(0, filePath.find_last_of('.')) + ".custom";
    ModelImporter::saveAsCustomFormat(go, customFilePath);

    Console::Instance().Log("GameObject imported and saved successfully.");
}


GameObject* SceneManager::getGameObject(int index) {
    return &gameObjectsOnScene[index];
}

void SceneManager::deleteSelectedObject() {
    if (selectedObject) {
        auto it = std::find_if(gameObjectsOnScene.begin(), gameObjectsOnScene.end(),
            [](const GameObject& obj) { return &obj == selectedObject; });
        if (it != gameObjectsOnScene.end()) {
            gameObjectsOnScene.erase(it);
            selectedObject = nullptr;
            Console::Instance().Log("Object deleted successfully.");
        }
    }
}

void SceneManager::LoadCustomModel(const std::string& filePath) {
    try {
        GameObject go = ModelImporter::loadCustomFormat(filePath);
        go.setName("Custom Model (" + std::to_string(gameObjectsOnScene.size()) + ")");
        gameObjectsOnScene.push_back(go);
        Console::Instance().Log("Custom model imported successfully.");
    }
    catch (const std::exception& e) {
        Console::Instance().Log(std::string("Failed to load custom model: ") + e.what());
    }
}

// SceneManager.cpp
void SceneManager::saveScene(const std::string& filePath) {
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        Console::Instance().Log("Failed to open file for saving scene: " + filePath);
        return;
    }

    // Write scene metadata (start as text or binary marker)
    outFile << "{\n\"GameObjects\": [\n";

    for (size_t i = 0; i < gameObjectsOnScene.size(); ++i) {
        const auto& go = gameObjectsOnScene[i];
        const auto& transform = go.GetComponent<TransformComponent>()->transform();

        outFile << "  {\n";
        outFile << "    \"UID\": " << go.getUUID() << ",\n";
        outFile << "    \"Name\": \"" << go.getName() << "\",\n";
        outFile << "    \"Transform\": {\n";
        outFile << "      \"Position\": [" << transform.pos().x << ", " << transform.pos().y << ", " << transform.pos().z << "],\n";
        outFile << "      \"Scale\": [" << transform.extractScale(transform.mat()).x << ", "
            << transform.extractScale(transform.mat()).y << ", "
            << transform.extractScale(transform.mat()).z << "],\n";
        outFile << "      \"Rotation\": [" << transform.extractEulerAngles(transform.mat()).x << ", "
            << transform.extractEulerAngles(transform.mat()).y << ", "
            << transform.extractEulerAngles(transform.mat()).z << "]\n";
        outFile << "    },\n";

        if (go.hasComponent<CameraComponent>()) {
            auto camera = go.GetComponent<CameraComponent>()->camera();
            outFile << "    \"Camera\": {\n";
            outFile << "      \"FOV\": " << camera.fov << ",\n";
            outFile << "      \"Aspect\": " << camera.aspect << ",\n";
            outFile << "      \"zNear\": " << camera.zNear << ",\n";
            outFile << "      \"zFar\": " << camera.zFar << "\n";
            outFile << "    },\n";
        }

        if (go.hasMesh()) {
            outFile << "    \"Mesh\": \"BinaryDataStart\"\n"; // Indicate binary section
            const auto& mesh = go.mesh();

            // Write binary mesh data
            uint32_t vertexCount = mesh.vertices().size();
            uint32_t indexCount = mesh.indices().size();

            outFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
            outFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
            outFile.write(reinterpret_cast<const char*>(mesh.vertices().data()), vertexCount * sizeof(glm::vec3));
            outFile.write(reinterpret_cast<const char*>(mesh.indices().data()), indexCount * sizeof(unsigned int));
        }
        else {
            outFile << "    \"Mesh\": null\n";
        }

        outFile << "  }";
        if (i != gameObjectsOnScene.size() - 1) outFile << ",";
        outFile << "\n";
    }

    outFile << "]\n}";
    outFile.close();

    Console::Instance().Log("Scene saved to " + filePath);
}

void SceneManager::loadScene(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        Console::Instance().Log("Failed to open file for loading scene: " + filePath);
        return;
    }

    gameObjectsOnScene.clear();

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.find("\"UID\":") != std::string::npos) {
            GameObject go;
            go.setUUID(std::stoi(line.substr(line.find(":") + 1)));

            // Parse Name
            std::getline(inFile, line);
            if (line.find("\"Name\":") != std::string::npos) {
                std::string name = line.substr(line.find(":") + 2);
                name.erase(name.find_last_of("\""));
                go.setName(name);
            }

            // Parse Transform
            Transform transform;
            while (std::getline(inFile, line) && line.find("}") == std::string::npos) {
                if (line.find("\"Position\":") != std::string::npos) {
                    std::istringstream iss(line.substr(line.find("[") + 1));
                    glm::vec3 pos;
                    char delim;
                    iss >> pos.x >> delim >> pos.y >> delim >> pos.z;
                    transform.translate(pos);
                }
                else if (line.find("\"Scale\":") != std::string::npos) {
                    std::istringstream iss(line.substr(line.find("[") + 1));
                    glm::vec3 scale;
                    char delim;
                    iss >> scale.x >> delim >> scale.y >> delim >> scale.z;
                    transform.setScale(scale);
                }
                else if (line.find("\"Rotation\":") != std::string::npos) {
                    std::istringstream iss(line.substr(line.find("[") + 1));
                    glm::vec3 rotation;
                    char delim;
                    iss >> rotation.x >> delim >> rotation.y >> delim >> rotation.z;
                    transform.setRotation(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
                }
            }
            go.AddComponent<TransformComponent>()->transform() = transform;

            if (line.find("\"Camera\":") != std::string::npos) {
                auto cameraComponent = go.AddComponent<CameraComponent>();
                while (std::getline(inFile, line) && line.find("}") == std::string::npos) {
                    if (line.find("\"FOV\":") != std::string::npos) {
                        cameraComponent->camera().fov = std::stod(line.substr(line.find(":") + 1));
                    }
                    else if (line.find("\"Aspect\":") != std::string::npos) {
                        cameraComponent->camera().aspect = std::stod(line.substr(line.find(":") + 1));
                    }
                    else if (line.find("\"zNear\":") != std::string::npos) {
                        cameraComponent->camera().zNear = std::stod(line.substr(line.find(":") + 1));
                    }
                    else if (line.find("\"zFar\":") != std::string::npos) {
                        cameraComponent->camera().zFar = std::stod(line.substr(line.find(":") + 1));
                    }
                }
            }

            // Parse Mesh
            std::getline(inFile, line);
            if (line.find("\"Mesh\": \"BinaryDataStart\"") != std::string::npos) {
                auto mesh = std::make_shared<Mesh>();

                uint32_t vertexCount, indexCount;
                inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
                inFile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

                std::vector<glm::vec3> vertices(vertexCount);
                std::vector<unsigned int> indices(indexCount);

                inFile.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(glm::vec3));
                inFile.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(unsigned int));

                mesh->load(vertices.data(), vertices.size(), indices.data(), indices.size());
                go.setMesh(mesh);
            }

            gameObjectsOnScene.push_back(go);
        }
    }

    inFile.close();
    Console::Instance().Log("Scene loaded from " + filePath);
}




