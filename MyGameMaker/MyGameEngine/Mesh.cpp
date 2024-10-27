#include "Mesh.h"
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include "Transform.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>
using namespace std;

#define CHECKERS_HEIGHT 32
#define CHECKERS_WIDTH 32

void Mesh::load(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs) {
	_vertices.assign(verts, verts + num_verts);
	_indices.assign(indexs, indexs + num_indexs);
	_vertexBuffer.loadElements(_vertices.size(), _vertices.data());
	_indexBuffer.loadIndices(_indices.size(), _indices.data());
	_texCoordBuffer.unload();
	_normalBuffer.unload();
	_colorBuffer.unload();

}

void Mesh::loadTexCoords(const glm::vec2* tex_coords, size_t num_tex_coords)
{
    _texCoordBuffer.loadData(num_tex_coords * sizeof(glm::vec2), tex_coords);
}

void Mesh::loadNormals(const glm::vec3* normals) {
	_normalBuffer.loadElements(_vertices.size(), normals);
}

void Mesh::loadColors(const glm::u8vec3* colors) {
	_colorBuffer.loadElements(_vertices.size(), colors);
}

void Mesh::draw() const {
    if (id_texture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, id_texture);
    }
	if (_texCoordBuffer.id()) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		_texCoordBuffer.bind();
		glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	}

	if (_normalBuffer.id()) {
		glEnableClientState(GL_NORMAL_ARRAY);
		_normalBuffer.bind();
		glNormalPointer( GL_FLOAT, 0, nullptr);
	}

	if (_colorBuffer.id()) {
		glEnableClientState(GL_COLOR_ARRAY);
		_colorBuffer.bind();
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, nullptr);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	_vertexBuffer.bind();
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	_indexBuffer.bind();
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(_indices.size()), GL_UNSIGNED_INT, nullptr);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (_colorBuffer.id()) glDisableClientState(GL_COLOR_ARRAY);
	if (_normalBuffer.id()) glDisableClientState(GL_NORMAL_ARRAY);
	if (_texCoordBuffer.id()) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if (id_texture)
    {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


void Mesh::LoadWhiteTexture() {
    GLubyte whiteImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            whiteImage[i][j][0] = 122; // Rojo
            whiteImage[i][j][1] = 89; // Verde
            whiteImage[i][j][2] = 56; // Azul
            whiteImage[i][j][3] = 255; // Alpha (opacidad)
        }
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &id_texture);
    glBindTexture(GL_TEXTURE_2D, id_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteImage);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::LoadCheckerTexture() {
    GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
            checkerImage[i][j][0] = (GLubyte)c;
            checkerImage[i][j][1] = (GLubyte)c;
            checkerImage[i][j][2] = (GLubyte)c;
            checkerImage[i][j][3] = (GLubyte)255;
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &id_texture);
    glBindTexture(GL_TEXTURE_2D, id_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}

// Función para cargar el archivo de la malla
void Mesh::loadFile(const char* file_path) {
    
    const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene != nullptr && scene->HasMeshes()) {
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            size_t num_vertices = scene->mMeshes[i]->mNumVertices;
            glm::vec3* vertex = new glm::vec3[num_vertices * 3];
            memcpy(vertex, scene->mMeshes[i]->mVertices, sizeof(float) * num_vertices * 3);

            if (scene->mMeshes[i]->HasTextureCoords(0)) {
                glm::vec2* texCoords = new glm::vec2[num_vertices];
                for (size_t j = 0; j < num_vertices; ++j) {
                    texCoords[j] = glm::vec2(
                        scene->mMeshes[i]->mTextureCoords[0][j].x,
                        scene->mMeshes[i]->mTextureCoords[0][j].y
                    );
                }
                loadTexCoords( texCoords, num_vertices);
                delete[] texCoords;
            }
            if (scene->mMeshes[i]->HasFaces()) {
                size_t num_index = scene->mMeshes[i]->mNumFaces * 3;
                unsigned int* index = new unsigned int[num_index]; // assume each face is a triangle
                for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; ++j) {
                    memcpy(&index[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(unsigned int));
                }
                load(vertex, num_vertices, index, num_index);
            }
            //Normals and colors (optional)
            //if (scene->mMeshes[i]->HasNormals()) {
            //    glm::vec2* normals = new glm::vec2[num_vertices];
            //    for (size_t j = 0; j < num_vertices; ++j) {
            //        normals[j] = glm::vec2(
            //            scene->mMeshes[i]->mTextureCoords[0][j].x,
            //            scene->mMeshes[i]->mTextureCoords[0][j].y
            //        );
            //    }
            //    loadTexCoords( normals, num_vertices);
            //    delete[] normals;
            //}
            //if (scene->mMeshes[i]->HasVertexColors(0)) {
            //    glm::vec2* colors = new glm::vec2[num_vertices];
            //    for (size_t j = 0; j < num_vertices; ++j) {
            //        colors[j] = glm::vec2(
            //            scene->mMeshes[i]->mTextureCoords[0][j].x,
            //            scene->mMeshes[i]->mTextureCoords[0][j].y
            //        );
            //    }
            //    loadTexCoords( colors, num_vertices);
            //    delete[] colors;
            //}
        }
        aiReleaseImport(scene);
    } 
    else {
        // Handle error
    }
}
            



void Mesh::loadTexture(const std::string& texture_path) {

    ILuint il_img_id;
    ilGenImages(1, &il_img_id);
    ilBindImage(il_img_id);
    ilLoadImage((const wchar_t*)texture_path.c_str());
    auto width = ilGetInteger(IL_IMAGE_WIDTH);

    auto height = ilGetInteger(IL_IMAGE_HEIGHT);

    auto channels = ilGetInteger(IL_IMAGE_CHANNELS);
    int img_bpp = ilGetInteger(IL_IMAGE_BPP);  // Bytes per pixel
    int img_format = ilGetInteger(IL_IMAGE_FORMAT);
    unsigned char* img_data = ilGetData();
    //load image as a texture in VRAM
    id_texture = _texCoordBuffer.id();
    glGenTextures(1, &id_texture);
    glBindTexture(GL_TEXTURE_2D, id_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, img_bpp, width, height, 0, img_format, GL_UNSIGNED_BYTE, img_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    //now we can delete image from RAM
    ilDeleteImage(il_img_id);

}