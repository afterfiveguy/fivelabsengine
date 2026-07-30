#pragma once

// clang-format off
#include <glad/glad.h>   // glad must be included before GLFW
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"

#include <fstream>
#include <sstream>
#include <string>

namespace fivelabsengine
{
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
}