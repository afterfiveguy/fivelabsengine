#include "app.h"
#include "shader.h"

#include <iostream>

namespace fivelabsengine {

namespace {
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
} // namespace

void App::run() {
  Shader ourShader("./src/shaders/simple.vs", "./src/shaders/simple.fs");

  float firstTriangleVertices[] = {
      // positions         // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
  };

  unsigned int VBOs[2], VAOs[2];
  glGenVertexArrays(2, VAOs);
  glGenBuffers(2, VBOs);

  glBindVertexArray(VAOs[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangleVertices),
               firstTriangleVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window.getGLFWwindow(),
                                 framebufferSizeCallback);
  glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

  while (!window.shouldClose()) {
    glClear(GL_COLOR_BUFFER_BIT);

    ourShader.use();

    float time = glfwGetTime();
    float greenValue = sin((time) / 2.0f) + 0.5f;

    int vertexColorLocation = glGetUniformLocation(ourShader.ID, "ourColor");

    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    window.swapBuffers();
    glfwPollEvents();
  }
}
} // namespace fivelabsengine
