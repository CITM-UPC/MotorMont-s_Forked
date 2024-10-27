#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "MyMesh.h"

#include <GL/glew.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>
#include <string>


#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

// Función para convertir de const char* a const wchar_t*
std::wstring ConvertToWString(const char* charStr) {
    size_t newSize = strlen(charStr) + 1;
    wchar_t* wString = new wchar_t[newSize];
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wString, newSize, charStr, _TRUNCATE);
    std::wstring wStr(wString);
    delete[] wString;
    return wStr;
}


void MyMesh::InitBuffers() {
    glGenBuffers(1, &id_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
    glBufferData(GL_ARRAY_BUFFER, num_vertex * 3 * sizeof(float), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &id_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, id_texcoord);
    glBufferData(GL_ARRAY_BUFFER, num_vertex * 2 * sizeof(float), texcoord, GL_STATIC_DRAW);

    glGenBuffers(1, &id_index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_index * sizeof(unsigned int), index, GL_STATIC_DRAW);
}
void MyMesh::Draw() const {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, id_texture);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Enlazar el buffer de vértices y definir el puntero
    glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    // Enlazar el buffer de coordenadas de textura y definir el puntero
    glBindBuffer(GL_ARRAY_BUFFER, id_texcoord);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    // Enlazar el buffer de índices y dibujar
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
    glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
}





// Destructor para liberar memoria
MyMesh::~MyMesh() {
    if (vertex != nullptr) {
        delete[] vertex;
    }
    if (index != nullptr) {
        delete[] index;
    }
}
