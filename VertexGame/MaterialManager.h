#pragma once
#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "Math.h"

namespace MaterialManager
{
	unsigned int LoadTexture(char const* path, bool FlipVertically);
	unsigned int LoadCubemap(std::vector<std::string> faces, bool FlipVertically);
}