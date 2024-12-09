#pragma once

#include <string>
#include "GameObject.h"

class ModelImporter {
public:
	static GameObject loadFromFile(const std::string& path);
};