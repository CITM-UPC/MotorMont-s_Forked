#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "BufferObject.h"
#include <string>

class Mesh {

	BufferObject _texCoordBuffer;
	BufferObject _normalBuffer;
	BufferObject _colorBuffer;
	BufferObject _vertexBuffer;
	BufferObject _indexBuffer;

	unsigned int id_index = 0;
	unsigned int num_index = 0;
	unsigned int* index = nullptr;

	unsigned int id_vertex = 0;
	unsigned int num_vertex = 0;
	unsigned int id_texcoord = 0;
	float* vertex = nullptr;
	float* texcoord = nullptr;

	unsigned int id_texture = 0;

	std::vector<glm::vec3> _vertices;
	std::vector<unsigned int> _indices;

public:
	const auto& vertices() const { return _vertices; }
	const auto& indices() const { return _indices; }

	void load(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs);
	void loadTexCoords(const glm::vec2* tex_coords, size_t num_tex_coords);
	void loadNormals(const glm::vec3* normals);
	void loadColors(const glm::u8vec3* colors);
	void draw() const;
	void loadFile(const char* file_path);
	void loadTexture(const std::string& texture_path);

	void LoadCheckerTexture();
	void LoadWhiteTexture();
};