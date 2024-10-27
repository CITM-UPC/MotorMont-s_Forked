#include "BufferObject.h"
#include <GL/glew.h>

BufferObject::BufferObject(BufferObject&& other) noexcept : _id(other._id), _target(other._target) {
	other._id = 0;
}

void BufferObject::loadData(int target, size_t size, const void* data) {
	if (!_id) glGenBuffers(1, &_id);
	_target = target;
	bind();
	glBufferData(_target, size, data, GL_STATIC_DRAW);
}

void BufferObject::loadData(size_t size, const void* data) {
	loadData(GL_ARRAY_BUFFER, size, data);
}

void BufferObject::loadIndices(size_t count, const unsigned int* data) {
	loadData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data);
}

void BufferObject::bind() const {
	glBindBuffer( _target, _id);
}

void BufferObject::unload() {
	if (_id) glDeleteBuffers(1, &_id);
	_id = 0;
}

BufferObject::~BufferObject() {
	unload();
}