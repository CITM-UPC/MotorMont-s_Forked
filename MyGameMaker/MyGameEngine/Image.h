#pragma once

#include <ostream>
#include <istream>
#include <vector>

class Image {

	unsigned int _id = 0;
	unsigned short _width = 0;
	unsigned short _height = 0;
	unsigned char _channels = 0;

	mutable std::vector<unsigned char> _dataCache;

public:
	unsigned int id() const { return _id; }
	auto width() const { return _width; }
	auto height() const { return _height; }
	auto channels() const { return _channels; }

	Image() = default;
	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&& other) noexcept;
	Image& operator=(Image&& other) noexcept = delete;
	~Image();

	void bind() const;
	void load(int width, int height, int channels, void* data);
	void loadTexture(const std::string& path);

	const std::vector<unsigned char>& rawData() const;
};

std::ostream& operator<<(std::ostream& os, const Image& img);
std::istream& operator>>(std::istream& is, Image& img);


