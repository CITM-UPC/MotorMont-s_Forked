#include "SceneManager.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Image.h"

std::vector<GameObject> SceneManager::gameObjectsOnScene;

void SceneManager::spawnBakerHouse() 
{
    GameObject go;
    auto mesh = std::make_shared<Mesh>();
    mesh->LoadFile("BakerHouse.fbx");
    go.setMesh(mesh);
	go.transform().pos() = vec3(4, 0, 0);
    SceneManager::gameObjectsOnScene.push_back(go);
}


void SceneManager::LoadGameObject(const std::string& filePath) {
    auto mesh = std::make_shared<Mesh>();

    GameObject go;
    mesh->LoadFile(filePath.c_str());
    go.setMesh(mesh);
    gameObjectsOnScene.push_back(go);
}

GameObject* SceneManager::getGameObject(int index) {
	return &gameObjectsOnScene[index];
}
