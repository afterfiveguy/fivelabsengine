#include "config.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  std::cout << "Welcome, fivelabsengine." << "\n";

  if (!glfwInit()) {
    std::cout << "GLFW Couldn't start" << "\n";
    return -1;
  }

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Fivelabs Engine", NULL, NULL);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}
