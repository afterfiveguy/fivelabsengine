#include "app.h"

#include <iostream>

namespace fivelabsengine {

namespace {
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
} // namespace

void App::run() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return;
  }

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window.getGLFWwindow(),
                                 framebufferSizeCallback);

  glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

  while (!window.shouldClose()) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);

    window.swapBuffers();
  }
}
} // namespace fivelabsengine
