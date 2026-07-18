#pragma once

#include <glad/glad.h>
// glad must be included before GLFW so GLFW doesn't pull in the system GL
// header
#include <GLFW/glfw3.h>
#include <string>

namespace fivelabsengine {
class Window {
public:
  Window(int w, int h, std::string name);
  ~Window();

  // A Window owns a raw GLFWwindow* resource. Copying it would let two Windows
  // destroy the same handle, so we forbid copies.
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); }
  void swapBuffers() { glfwSwapBuffers(window); }

  // Hand out the underlying handle for the few GLFW calls that need it
  // directly.
  GLFWwindow *getGLFWwindow() const { return window; }

private:
  void initWindow();

  const int width;
  const int height;

  std::string windowName;
  GLFWwindow *window;
};
} // namespace fivelabsengine
