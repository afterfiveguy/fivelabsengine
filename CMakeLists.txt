cmake_minimum_required(VERSION 3.16)
project(fivelabsengine VERSION 1.0.0)

# Emit compile_commands.json so clangd (Neovim LSP) resolves includes
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ standard that both the compiler and clangd should assume
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(fivelabsengine
  src/main.cpp
  src/window.cpp
  # src/debug-log.cpp
  src/shader.cpp
  src/app.cpp
  src/glad.c
  # headers listed so they show up in IDEs / get a compile command context
  src/config.h
  src/window.h
  # src/debug-log.h
  src/app.h
  src/shader.h
  src/stb_image.h
  src/stb_image.cpp
)

target_include_directories(fivelabsengine PRIVATE dependencies src)
target_link_libraries(fivelabsengine OpenGL::GL)
target_link_libraries(fivelabsengine glfw)
target_link_libraries(fivelabsengine glm::glm)
