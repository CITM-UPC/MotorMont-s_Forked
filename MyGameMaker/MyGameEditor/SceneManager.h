#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "MyGameEngine/GameObject.h"
class SceneManager
{
public:
	static void LoadGameObject(const std::string& filePath);
	static void spawnBakerHouse();
	static GameObject* getGameObject(int index);
    //File drop handler
	static void deleteSelectedObject(); // Nueva función para eliminar el objeto seleccionado

public: 
	static std::vector<GameObject> gameObjectsOnScene;
	static GameObject* selectedObject;

};

