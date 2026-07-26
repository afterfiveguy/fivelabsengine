#include "app.h"
#include "shader.h"
#include "stb_image.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace fivelabsengine
{

  namespace
  {
    float mixValue = 0.1f;

    void framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
      glViewport(0, 0, width, height);
    }

    unsigned int loadTexture(const char *path, bool flipVertically = true,
                             GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT,
                             GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
                             GLenum magFilter = GL_LINEAR)
    {
      unsigned int id;

      glGenTextures(1, &id);
      glBindTexture(GL_TEXTURE_2D, id);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

      int width, height, nrChannels;

      unsigned char *data = stbi_load(path,
                                      &width, &height, &nrChannels, 0);

      stbi_set_flip_vertically_on_load(flipVertically);
      GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
      if (!data)
      {
        std::cout << "Failed to load texture" << std::endl;
        return id;
      }
      std::cout << "Loaded texture container: " << width << "x" << height << "\n";
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      stbi_image_free(data);
      return id;
    }
    void processInput(GLFWwindow *window)
    {
      if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

      if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      {
        std::cout << "Mix value: " << mixValue << std::endl;
        mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue >= 1.0f)
          mixValue = 1.0f;
      }
      if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      {
        std::cout << "Mix value: " << mixValue << std::endl;
        mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue <= 0.0f)
          mixValue = 0.0f;
      }
    }
  } // namespace

  void App::run()
  {
    Shader ourShader("./src/shaders/simple.vs", "./src/shaders/simple.fs");

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window.getGLFWwindow(),
                                   framebufferSizeCallback);
    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

    // Loading texture

    const unsigned int texture1 = loadTexture("./src/resources/textures/container.jpg", false, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    const unsigned int texture2 = loadTexture("./src/resources/textures/awesomeface.png", true, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // End of texture loading
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    while (!window.shouldClose())
    {
      processInput(window.getGLFWwindow());

      ourShader.setFloat("mixValue", mixValue);

      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture2);

      glBindVertexArray(VAO);
      glm::mat4 transform = glm::mat4(1.0f);
      transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
      transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

      unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      transform = glm::mat4(1.0f);
      transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));
      float scaleAmount = static_cast<float>(sin(glfwGetTime()));
      transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]); // this time take the matrix value array's first element as its memory pointer value

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      window.swapBuffers();
      glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }
} // namespace fivelabsengine
