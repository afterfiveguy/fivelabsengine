cmake_minimum_required(VERSION 3.16)
project(fivelabsengine VERSION 1.0.0)

# Emit compile_commands.json so clangd (Neovim LSP) resolves includes
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ standard that both the compiler and clangd should assume
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

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
)

target_include_directories(fivelabsengine PRIVATE dependencies src)
target_link_libraries(fivelabsengine OpenGL::GL)
target_link_libraries(fivelabsengine glfw)

add_custom_command(TARGET fivelabsengine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/src/shaders
          $<TARGET_FILE_DIR:fivelabsengine>/shaders)