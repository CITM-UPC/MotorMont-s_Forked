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

    // Asegúrate de obtener el componente TransformComponent y luego acceder a Transform
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

    // Asegúrate de obtener el componente TransformComponent y luego acceder a Transform
    auto parentTransformComponent = parentObject->GetComponent<TransformComponent>();
    if (parentTransformComponent) {
        parentTransformComponent->transform().translate(glm::vec3(0, 0, 2)); // Posición en z = 2
    }

    // Crear el objeto hijo
    auto childObject = std::make_shared<GameObject>("ChildObject");
    auto childMesh = BasicShapesManager::MakeTriangleMesh(0.5);
    childObject->setMesh(childMesh);

    // Asegúrate de obtener el componente TransformComponent y luego acceder a Transform
    auto childTransformComponent = childObject->GetComponent<TransformComponent>();
    if (childTransformComponent) {
        childTransformComponent->transform().translate(glm::vec3(0, 0, 2.4)); // Posición en z = 2.4
    }

    // Parentar el objeto hijo al objeto padre
    parentObject->addChild(childObject);

    // Añadir el objeto padre a la escena
    SceneManager::gameObjectsOnScene.push_back(*parentObject);

    // Añadir el objeto hijo a la escena (opcional, si quieres que aparezca en la lista de objetos de la escena)
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
