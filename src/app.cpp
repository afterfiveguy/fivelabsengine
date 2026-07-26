#include "app.h"
#include "cube.h"
#include "shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iterator>
#include <vector>
namespace fivelabsengine {

namespace {
float mixValue = 0.1f;
float fov = 70.0f;
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

unsigned int loadTexture(const char *path, bool flipVertically = true,
                         GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT,
                         GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
                         GLenum magFilter = GL_LINEAR) {
  unsigned int id;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(flipVertically);
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (!data) {
    std::cout << "Failed to load texture" << std::endl;
    return id;
  }

  GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

  std::cout << "Loaded texture container: " << width << "x" << height << "\n";
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
  return id;
}
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    fov -= 0.1f;
    if (fov <= 1.0f)

      fov = 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    fov += 0.1f;
    if (fov >= 90.0f)

      fov = 90.0f;
  }

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    std::cout << "Mix value: " << mixValue << std::endl;
    mixValue += 0.001f; // change this value accordingly (might be too slow or
                        // too fast based on system hardware)
    if (mixValue >= 1.0f)
      mixValue = 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    std::cout << "Mix value: " << mixValue << std::endl;
    mixValue -= 0.001f; // change this value accordingly (might be too slow or
                        // too fast based on system hardware)
    if (mixValue <= 0.0f)
      mixValue = 0.0f;
  }
}
} // namespace

void App::run() {
  Shader ourShader("./src/shaders/simple.vs", "./src/shaders/simple.fs");

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window.getGLFWwindow(),
                                 framebufferSizeCallback);
  glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

  // Loading texture
  // Each Cube owns the container texture it binds to unit 0; the face that gets
  // mixed on top is shared, so it stays here on unit 1.
  const unsigned int texture2 =
      loadTexture("./src/resources/textures/awesomeface.png", true, GL_REPEAT,
                  GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

  // End of texture loading
  ourShader.use();
  ourShader.setInt("texture1", 0);
  ourShader.setInt("texture2", 1);

  glm::vec3 cubePositions[] = {
      glm::vec3(2.0f, 5.0f, -15.0f),   glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f, 3.0f, -7.5f),   glm::vec3(1.3f, -2.0f, -2.5f),
      glm::vec3(1.5f, 2.0f, -2.5f),    glm::vec3(1.5f, 0.2f, -1.5f),
      glm::vec3(-1.3f, 1.0f, -1.5f)};

  std::vector<Cube> cubes;
  cubes.reserve(std::size(cubePositions));
  for (unsigned int i = 0; i < std::size(cubePositions); i++)
    // stagger the starting orientation so they don't all spin in lockstep
    cubes.emplace_back(cubePositions[i], glm::vec3(1.0f),
                       glm::vec3(20.0f * i));

  glEnable(GL_DEPTH_TEST);
  float lastFrame = glfwGetTime();

  while (!window.shouldClose()) {
    const float now = glfwGetTime();
    const float dt = now - lastFrame;
    lastFrame = now;

    processInput(window.getGLFWwindow());

    ourShader.setFloat("mixValue", mixValue);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
    glm::mat4 projection = glm::perspective(
        glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    unsigned int projectionLoc =
        glGetUniformLocation(ourShader.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    for (Cube &cube : cubes) {
      cube.update(dt);
      cube.draw(ourShader.ID);
    }

    window.swapBuffers();
    glfwPollEvents();
  }
}
} // namespace fivelabsengine
