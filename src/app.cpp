#include "app.h"

#include <iostream>

#include <cube.h>
#include <camera.h>
namespace fivelabsengine
{

  namespace
  {
    void framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
      glViewport(0, 0, width, height);

      auto *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
      if (camera && height > 0)
        camera->setAspect((float)width / (float)height);
    }
    void mouseCallback(GLFWwindow *window, double xpos, double ypos)
    {
      auto *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
      if (camera)
        camera->onMouseMove(xpos, ypos);
    }
    void processInput(GLFWwindow *window, Camera *camera, float dt)
    {
      if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->move(CameraMovement::Forward, dt);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->move(CameraMovement::Backward, dt);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->move(CameraMovement::Left, dt);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->move(CameraMovement::Right, dt);
    }

  } // namespace

  void App::run()
  {
    Shader ourShader("./src/shaders/simple.vs", "./src/shaders/simple.fs");

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

    ourShader.use();

    Cube *cube = new Cube({0.0f, 0.0f, 0.0f}, {0.25f, 0.25f, 0.25f});
    Cube *cube2 = new Cube({1.1f, 0.1f, 0.1f}, {0.25f, 0.25f, 0.25f});
    Cube *cube3 = new Cube({-1.1f, -0.1f, -0.1f}, {0.25f, 0.25f, 0.25f});
    Camera *camera = new Camera({0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});

    // The camera must be registered before the callbacks, since both of them
    // reach it through the window user pointer.
    glfwSetWindowUserPointer(window.getGLFWwindow(), camera);
    glfwSetFramebufferSizeCallback(window.getGLFWwindow(),
                                   framebufferSizeCallback);
    glfwSetCursorPosCallback(window.getGLFWwindow(), mouseCallback);
    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float lastFrame = 0.0f;
    while (!window.shouldClose())
    {
      float now = static_cast<float>(glfwGetTime());
      float dt = now - lastFrame;
      lastFrame = now;

      processInput(window.getGLFWwindow(), camera, dt);
      glEnable(GL_DEPTH_TEST);
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      ourShader.setMat4("projection", camera->getProjection());
      ourShader.setMat4("view", camera->getView());

      cube->update(dt);
      cube->draw(ourShader.ID);
      cube2->update(dt);
      cube2->draw(ourShader.ID);
      cube3->update(dt);
      cube3->draw(ourShader.ID);
      window.swapBuffers();
      glfwPollEvents();
    }

    delete cube;
    delete cube2;
    delete cube3;
    delete camera;
  }
} // namespace fivelabsengine
