#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../MyGameEngine/GameObject.h"

class SceneManager
{
public:
	static void LoadGameObject(const std::string& filePath);
	static void LoadCustomModel(const std::string& filePath);

	static void spawnBakerHouse();
	static GameObject* getGameObject(int index);
    //File drop handler
	static void deleteSelectedObject(); // Nueva funci�n para eliminar el objeto seleccionado
	static void spawnParentedObjects(); // Nueva funci�n para generar objetos parenteados

	static std::string getFileDirectory(const std::string& filePath);
	static std::string getFileNameWithoutExtension(const std::string& filePath);


public: 
	static std::vector<GameObject> gameObjectsOnScene;
	static GameObject* selectedObject;
};

