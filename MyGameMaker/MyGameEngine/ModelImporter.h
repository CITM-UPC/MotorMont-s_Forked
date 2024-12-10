#pragma once

#include <string>
#include "GameObject.h"

class ModelImporter {
public:
	static GameObject loadFromFile(const std::string& path);
	static GameObject loadCustomFormat(const std::string& path);
	static void saveAsCustomFormat(const GameObject& gameObject, const std::string& outputPath);
};