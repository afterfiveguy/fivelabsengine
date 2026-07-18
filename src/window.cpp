#include "window.h"

namespace fivelabsengine {
Window::Window(int w, int h, std::string name)
    : width{w}, height{h}, windowName{name} {
  initWindow();
}
Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // required on macOS
#endif

  window =
      glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

  glfwMakeContextCurrent(window);
}
} // namespace fivelabsengine
