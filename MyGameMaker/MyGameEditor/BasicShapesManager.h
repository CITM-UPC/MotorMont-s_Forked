#pragma once
#include <iostream>
#include "MyGameEngine/GameObject.h"

class BasicShapesManager {
public:
    static std::shared_ptr<Mesh> MakeTriangleMesh(double size);
    static std::shared_ptr<Mesh> MakeQuadMesh(double size);
    static std::shared_ptr<Mesh> MakeCubeMesh(double size);

    // A�adimos las coordenadas del rat�n
    static void createFigure(int figureType, std::vector<GameObject>& gameObjects, double size = 1.0, glm::vec3 mousePosition = glm::vec3(0.0f,0.0f,0.0f));
};
