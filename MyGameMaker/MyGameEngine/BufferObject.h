#pragma once

#include <typeinfo>

class BufferObject {

	unsigned int _id = 0;
	int _target = 0x8892;

public:
	const auto& id() const { return _id; }
	bool isLoaded() const { return _id != 0; }

	BufferObject() = default;
	BufferObject(const BufferObject&) = delete;
	BufferObject& operator=(const BufferObject&) = delete;
	BufferObject(BufferObject&& other) noexcept;
	BufferObject& operator=(BufferObject&& other) noexcept = delete;
	~BufferObject();

	void loadData(size_t size, const void* data);
	void loadIndices(size_t count, const unsigned int* data);
	template <typename T> void loadElements(size_t count, const T* data) { loadData(count * sizeof(T), data); }

	void bind() const;
	void unload();

public:
	void loadData(int target, size_t size, const void* data);

};

