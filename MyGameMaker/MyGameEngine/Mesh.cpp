#include "Mesh.h"

#include <GL/glew.h>
using namespace std;

#define CHECKERS_HEIGHT 32
#define CHECKERS_WIDTH 32

Mesh::Mesh()
{
	//_meshLoader = new MeshLoader();
}

void Mesh::load(const glm::vec3* vertices, size_t num_verts, unsigned int* indices, size_t num_indexs)
{
	_vertices.assign(vertices, vertices + num_verts);
	_indices.assign(indices, indices + num_indexs);
	_vertexBuffer.loadData(vertices, num_verts * sizeof(glm::vec3));
	_indexBuffer.loadIndices(indices, num_indexs);
	_texCoordsBuffer.unload();
	_normalsBuffer.unload();
	_colorsBuffer.unload();

	_boundingBox.min = _vertices.front();
	_boundingBox.max = _vertices.front();

	for (const auto& v : _vertices) {
		_boundingBox.min = glm::min(_boundingBox.min, glm::dvec3(v));
		_boundingBox.max = glm::max(_boundingBox.max, glm::dvec3(v));
	}
}

void Mesh::loadTexCoords(const glm::vec2* tex_coords, size_t num_tex_coords)
{
	_texCoordsBuffer.loadData(tex_coords, num_tex_coords * sizeof(glm::vec2));
}

void Mesh::loadNormals(const glm::vec3* normals, size_t num_normals)
{
	_normalsBuffer.loadData(normals, num_normals * sizeof(glm::vec3));
}

void Mesh::loadColors(const glm::u8vec3* colors, size_t num_colors)
{
	_colorsBuffer.loadData(colors, num_colors * sizeof(glm::u8vec3));
}

void Mesh::draw() const
{

	if (_idTexture)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _idTexture);
	}

	if (_texCoordsBuffer.id())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		_texCoordsBuffer.bind();
		glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	}

	if (_normalsBuffer.id())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		_normalsBuffer.bind();
		glNormalPointer(GL_FLOAT, 0, nullptr);
	}

	if (_colorsBuffer.id())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		_colorsBuffer.bind();
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, nullptr);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	_vertexBuffer.bind();
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	_indexBuffer.bind();
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (_colorsBuffer.id()) glDisableClientState(GL_COLOR_ARRAY);
	if (_normalsBuffer.id()) glDisableClientState(GL_NORMAL_ARRAY);
	if (_texCoordsBuffer.id()) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (_idTexture)
	{
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::LoadFile(const char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) {
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			size_t num_vertices = scene->mMeshes[i]->mNumVertices;
			glm::vec3* vertex = new glm::vec3[num_vertices * 3];
			memcpy(vertex, scene->mMeshes[i]->mVertices, sizeof(float) * num_vertices * 3);


			if (scene->mMeshes[i]->HasFaces()) {
				size_t num_index = scene->mMeshes[i]->mNumFaces * 3;
				unsigned int* index = new unsigned int[num_index]; // assume each face is a triangle
				for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; ++j) {
					memcpy(&index[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(unsigned int));
				}
				load(vertex, num_vertices, index, num_index);
			}

			if (scene->mMeshes[i]->HasTextureCoords(0)) {
				glm::vec2* texCoords = new glm::vec2[num_vertices];
				for (size_t j = 0; j < num_vertices; ++j) {
					texCoords[j] = glm::vec2(
						scene->mMeshes[i]->mTextureCoords[0][j].x,
						-scene->mMeshes[i]->mTextureCoords[0][j].y
					);
				}
				loadTexCoords(texCoords, num_vertices);
				delete[] texCoords;
			}


		}
		aiReleaseImport(scene);


	}
	else {
		// Handle error
	}
}