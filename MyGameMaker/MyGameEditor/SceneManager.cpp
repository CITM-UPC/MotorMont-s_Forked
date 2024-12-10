#include "SceneManager.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Image.h"
#include "Console.h"
#include "MyGameEngine/ModelImporter.h"
#include "MyGameEngine/ImageImporter.h"
#include "MyGameEngine/Texture.h"
#include "BasicShapesManager.h"

#include "SceneManager.h"
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include "MyGui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

std::string SceneManager::getFileDirectory(const std::string& filePath) {
    return std::filesystem::path(filePath).parent_path().string();
}

std::string SceneManager::getFileNameWithoutExtension(const std::string& filePath) {
    return std::filesystem::path(filePath).stem().string();
}

std::vector<GameObject> SceneManager::gameObjectsOnScene;
GameObject* SceneManager::selectedObject = nullptr;


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

    // Aseg�rate de obtener el componente TransformComponent y luego acceder a Transform
    auto transformComponent = go.GetComponent<TransformComponent>();
    if (transformComponent) {
        transformComponent->transform().translate(glm::vec3(0, 0, 7));
    }

    go.setName("GameObject (" + std::to_string(gameObjectsOnScene.size()) + ")");
    SceneManager::gameObjectsOnScene.push_back(go);
}

void SceneManager::spawnParentedObjects()
{
    // Crear el objeto padre
    auto parentObject = std::make_shared<GameObject>("ParentObject");
    auto parentMesh = BasicShapesManager::MakeTriangleMesh(1.0);
    parentObject->setMesh(parentMesh);

    // Aseg�rate de obtener el componente TransformComponent y luego acceder a Transform
    auto parentTransformComponent = parentObject->GetComponent<TransformComponent>();
    if (parentTransformComponent) {
        parentTransformComponent->transform().translate(glm::vec3(0, 0, 2)); // Posici�n en z = 2
    }

    // Crear el objeto hijo
    auto childObject = std::make_shared<GameObject>("ChildObject");
    auto childMesh = BasicShapesManager::MakeTriangleMesh(0.5);
    childObject->setMesh(childMesh);

    // Aseg�rate de obtener el componente TransformComponent y luego acceder a Transform
    auto childTransformComponent = childObject->GetComponent<TransformComponent>();
    if (childTransformComponent) {
        childTransformComponent->transform().translate(glm::vec3(0, 0, 2.4)); // Posici�n en z = 2.4
    }

    // Parentar el objeto hijo al objeto padre
    parentObject->addChild(childObject);

    // A�adir el objeto padre a la escena
    SceneManager::gameObjectsOnScene.push_back(*parentObject);

    // A�adir el objeto hijo a la escena (opcional, si quieres que aparezca en la lista de objetos de la escena)
    SceneManager::gameObjectsOnScene.push_back(*childObject);
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

void SceneManager::saveScene(const std::string& filePath) {
    std::ofstream outFile(filePath, std::ios::out);
    if (!outFile) {
        Console::Instance().Log("Failed to open file for saving scene: " + filePath);
        return;
    }

    outFile << "{\n\"GameObjects\": [\n";
    for (size_t i = 0; i < gameObjectsOnScene.size(); ++i) {
        const auto& go = gameObjectsOnScene[i];
        const auto& transform = go.GetComponent<TransformComponent>()->transform();

        outFile << "  {\n";
        outFile << "    \"UID\": " << go.getUUID() << ",\n";
        outFile << "    \"ParentUID\": " << (go.GetParent() ? go.GetParent()->getUUID() : -1) << ",\n";
        outFile << "    \"Name\": \"" << go.getName() << "\",\n";
        outFile << "    \"Transform\": {\n";
        outFile << "      \"Position\": [" << transform.pos().x << ", " << transform.pos().y << ", " << transform.pos().z << "],\n";
        outFile << "      \"Scale\": [" << transform.extractScale(transform.mat()).x << ", "
            << transform.extractScale(transform.mat()).y << ", "
            << transform.extractScale(transform.mat()).z << "],\n";
        outFile << "      \"Rotation\": [" << transform.extractEulerAngles(transform.mat()).x << ", "
            << transform.extractEulerAngles(transform.mat()).y << ", "
            << transform.extractEulerAngles(transform.mat()).z << "]\n";
        outFile << "    }\n";
        outFile << "  }";
        if (i != gameObjectsOnScene.size() - 1) outFile << ",";
        outFile << "\n";
    }
    outFile << "]\n}";
    outFile.close();

    Console::Instance().Log("Scene saved to " + filePath);
}

void SceneManager::loadScene(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::in);
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

            gameObjectsOnScene.push_back(go);
        }
    }
    inFile.close();

    Console::Instance().Log("Scene loaded from " + filePath);
}