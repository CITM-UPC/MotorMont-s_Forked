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
	static void deleteSelectedObject(); // Nueva función para eliminar el objeto seleccionado
	static void spawnParentedObjects(); // Nueva función para generar objetos parenteados

	static std::string getFileDirectory(const std::string& filePath);
	static std::string getFileNameWithoutExtension(const std::string& filePath);

	static void saveScene(const std::string& filePath);
	static void loadScene(const std::string& filePath);
	static void startPlayback();
	static void stopPlayback();
	static void restoreSceneState(const std::string& filePath);
	static void saveSceneState(const std::string& filePath);



public: 
	static std::vector<GameObject> gameObjectsOnScene;
	static GameObject* selectedObject;
	

private:

	static void saveSceneState();
	static void restoreSceneState();
	static std::vector<Transform> initialTransforms;


	static std::vector<GameObject> savedSceneState;
	static bool isPlaying;


};

