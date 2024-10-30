#include "GameObject.h"
#include <GL/glew.h>
#include <glm/gtc/epsilon.hpp>

void GameObject::draw() const {
	glPushMatrix();
	glMultMatrixd(_transform.data());
	glColor3ubv(&_color.r);

	if (hasTexture()) {
		glEnable(GL_TEXTURE_2D);
		_texture.bind();
	}

	if (hasMesh()) _mesh_ptr->draw();
	
	if (hasTexture()) glDisable(GL_TEXTURE_2D);

	for (const auto& child : children()) child.draw();

	glPopMatrix();
}

BoundingBox GameObject::localBoundingBox() const {
	if (children().size()) {
		BoundingBox bbox = _mesh_ptr ? _mesh_ptr->boundingBox() : children().front().boundingBox();
		for (const auto& child : children()) bbox = bbox + child.boundingBox();
		return bbox;
	}
	else return _mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox();
}

BoundingBox GameObject::worldBoundingBox() const {
	BoundingBox bbox = worldTransform().mat() * (_mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox());
	for (const auto& child : children()) bbox = bbox + child.worldBoundingBox();
	return bbox;
}
std::vector<glm::vec3> GameObject::calculateNormalsPerTriangle() const {
    std::vector<glm::vec3> normals;

    // Verifica si la malla existe
    if (!_mesh_ptr) return normals;

    // Obtener los vértices de la malla (suponemos que devuelve los vértices en grupos de 3 para cada triángulo)
    const auto& vertices = _mesh_ptr->vertices();

    // Itera en los vértices en grupos de 3
    for (size_t i = 0; i < vertices.size(); i += 3) {
        // Obtener los vértices del triángulo
        const glm::vec3& v0 = vertices[i];
        const glm::vec3& v1 = vertices[i + 1];
        const glm::vec3& v2 = vertices[i + 2];

        // Calcular los vectores de los lados del triángulo
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        // Calcular la normal como el producto cruzado de edge1 y edge2
        glm::vec3 normal = glm::cross(edge1, edge2);

        // Comprobar si la longitud de la normal es cercana a cero antes de normalizar
        float length = glm::length(normal);
        if (length > 1e-6f) {  // Usamos un pequeño umbral, por ejemplo 1e-6
            normal /= length;  // Normalizar la normal dividiendo por su longitud
        }
        else {
            normal = glm::vec3(0.0f);  // Evitar la normalización si la longitud es cero
        }

        // Agregar la normal a la lista de normales
        normals.push_back(normal);
    }

    return normals;
}
// Función para dibujar una línea en OpenGL entre dos puntos 3D
void GameObject::drawLine( glm::vec3& start,  glm::vec3& end) {
    glBegin(GL_LINES); // Comienza a dibujar líneas
    glVertex3f(start.x, start.y, start.z); // Punto inicial de la línea
    glVertex3f(end.x, end.y, end.z);       // Punto final de la línea
    glEnd(); // Finaliza el dibujo de la línea
}
// Función para dibujar las normales por triángulo
void GameObject::drawNormals()  {
    if (!_mesh_ptr) return;

    // Obtener los vértices y las normales de cada triángulo
    const auto& vertices = _mesh_ptr->vertices();
    std::vector<glm::vec3> normals = calculateNormalsPerTriangle();

    // Obtener la matriz de transformación global del objeto
    glm::mat4 modelMatrix = worldTransform().mat();

    // Iterar en los triángulos
    for (size_t i = 0; i < vertices.size(); i += 3) {
        // Transformar cada vértice del triángulo al espacio mundial
        glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(vertices[i], 1.0f));
        glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(vertices[i + 1], 1.0f));
        glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(vertices[i + 2], 1.0f));

        // Calcular el centro del triángulo en el espacio mundial
        glm::vec3 center = (v0 + v1 + v2) / 3.0f;

        // Obtener la normal correspondiente
        const glm::vec3& normal = normals[i / 3];

        // Escalar la normal para hacerla visible al dibujar
        glm::vec3 normalEnd = center + normal * 0.1f;

        // Dibujar la línea desde el centro del triángulo en dirección a la normal
        drawLine(center, normalEnd);
    }
}
// Método para calcular y devolver las normales por vértice (promedio por cara)
std::vector<glm::vec3> GameObject::calculateNormalsPerFace() {
    if (!_mesh_ptr) return {};

    const auto& vertices = _mesh_ptr->vertices();
    std::vector<glm::vec3> vertexNormals(vertices.size(), glm::vec3(0.0f));

    // Recorrer los triángulos y acumular las normales para cada vértice
    for (size_t i = 0; i < vertices.size(); i += 3) {
        // Obtener los vértices del triángulo
        const glm::vec3& v0 = vertices[i];
        const glm::vec3& v1 = vertices[i + 1];
        const glm::vec3& v2 = vertices[i + 2];

        // Calcular los bordes del triángulo y su normal
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        // Acumular la normal de la cara en cada vértice del triángulo
        vertexNormals[i] += faceNormal;
        vertexNormals[i + 1] += faceNormal;
        vertexNormals[i + 2] += faceNormal;
    }

    // Normalizar todas las normales de los vértices acumulados
    for (auto& normal : vertexNormals) {
        if (glm::length(normal) > 1e-6f) { // Evitar la división por cero
            normal = glm::normalize(normal);
        }
    }

    return vertexNormals;
}

// Función para dibujar las normales por cara (promedio de vértices)
void GameObject::drawFaceNormals() {
    if (!_mesh_ptr) return;

    const auto& vertices = _mesh_ptr->vertices();
    std::vector<glm::vec3> vertexNormals = calculateNormalsPerFace();

    // Obtener la matriz de transformación global del objeto
    glm::mat4 modelMatrix = worldTransform().mat();

    // Dibujar cada normal desde su vértice
    for (size_t i = 0; i < vertices.size(); ++i) {
        // Transformar el vértice al espacio mundial
        glm::vec3 transformedVertex = glm::vec3(modelMatrix * glm::vec4(vertices[i], 1.0f));
        glm::vec3 normalEnd = transformedVertex + vertexNormals[i] * 0.1f;

        // Dibujar la línea representando la normal desde el vértice
        drawLine(transformedVertex, normalEnd);
    }
}

