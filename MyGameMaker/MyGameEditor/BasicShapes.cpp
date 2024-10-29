#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/GameObject.h"

// Gestor de figuras geométricas básicas
class BasicShapesManager {
public:
    static std::shared_ptr<Mesh> MakeTriangleMesh(double size) {
        const glm::vec3 vertices[] = { glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(0, size, 0) };
        unsigned int indices[] = { 0, 1, 2 };
        const glm::vec2 texcoords[] = { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(0.5, 0) };
        const glm::vec3 normals[] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
        const glm::u8vec3 colors[] = { glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255) };

        auto mesh_ptr = std::make_shared<Mesh>();
        mesh_ptr->load(vertices, 3, indices, 3);
        mesh_ptr->loadTexCoords(texcoords, texcoords->length());
        mesh_ptr->loadNormals(normals, normals->length());
        mesh_ptr->loadColors(colors, colors->length());
        return mesh_ptr;
    }

    static std::shared_ptr<Mesh> MakeQuadMesh(double size) {
        const glm::vec3 vertices[4] = { glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(size, size, 0), glm::vec3(-size, size, 0) };
        unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };
        const glm::vec2 texcoords[4] = { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0) };
        const glm::vec3 normals[4] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
        const glm::u8vec3 colors[4] = { glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255), glm::u8vec3(255, 255, 0) };

        auto mesh_ptr = std::make_shared<Mesh>();
        mesh_ptr->load(vertices, 4, indices, 6);
        mesh_ptr->loadTexCoords(texcoords, texcoords->length());
        mesh_ptr->loadNormals(normals, normals->length());
        mesh_ptr->loadColors(colors, colors->length());
        return mesh_ptr;
    }

    // Crea la figura solicitada y la añade a la lista de gameObjects
    static void createFigure(int figureType, std::vector<GameObject>& gameObjects, double size = 1.0) {
        GameObject go;
        switch (figureType) {
        case 1:  // Triángulo
            go.setMesh(MakeTriangleMesh(size));
            break;
        case 2:  // Cuadrado
            go.setMesh(MakeQuadMesh(size));
            break;
            // Agregar más figuras si es necesario con case 3, 4, etc.
        default:
            std::cout << "Figure type not recognized." << std::endl;
            return;
        }
        gameObjects.push_back(go);
    }
};
