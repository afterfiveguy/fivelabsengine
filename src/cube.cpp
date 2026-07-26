#include "cube.h"
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace fivelabsengine
{
  Cube::Cube(glm::vec3 position, glm::vec3 size, glm::vec3 eulers)
      : position{position}, size{size}, eulers{eulers},
        VAO{0}, VBO{0}, EBO{0}, texture{0}
  {
    make_mesh();
    make_texture();
  }

  Cube::~Cube() { release(); }

  Cube::Cube(Cube &&o) noexcept
      : position{o.position}, size{o.size}, eulers{o.eulers},
        VAO{o.VAO}, VBO{o.VBO}, EBO{o.EBO}, texture{o.texture}
  {
    // zero the source so its destructor doesn't delete handles we now own
    o.VAO = o.VBO = o.EBO = o.texture = 0;
  }

  Cube &Cube::operator=(Cube &&o) noexcept
  {
    if (this != &o)
    {
      release();

      position = o.position;
      size = o.size;
      eulers = o.eulers;
      VAO = o.VAO;
      VBO = o.VBO;
      EBO = o.EBO;
      texture = o.texture;

      o.VAO = o.VBO = o.EBO = o.texture = 0;
    }
    return *this;
  }

  void Cube::release()
  {
    // glDelete* on 0 is a no-op, so this is safe on a moved-from Cube
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);
    VAO = VBO = EBO = texture = 0;
  }

  void Cube::update(float dt)
  {
    // spin around roughly the same axis the cubes used before, in degrees/sec
    eulers += glm::vec3(1.0f, 0.3f, 0.5f) * 25.0f * dt;
    eulers = glm::mod(eulers, glm::vec3(360.0f));
  }

  void Cube::draw(unsigned int shader)
  {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model =
        glm::rotate(model, glm::radians(eulers.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model =
        glm::rotate(model, glm::radians(eulers.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model =
        glm::rotate(model, glm::radians(eulers.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, size);

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  unsigned int Cube::loadTexture(const char *path, bool flipVertically,
                                 GLenum wrapS, GLenum wrapT, GLenum minFilter,
                                 GLenum magFilter)
  {
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
    if (!data)
    {
      std::cout << "Failed to load texture: " << path << std::endl;
      return id;
    }

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    std::cout << "Loaded texture " << path << ": " << width << "x" << height
              << "\n";
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return id;
  }

  void Cube::make_mesh()
  {
    // 6 faces * 6 vertices, each vertex = 3 position floats + 2 texcoord floats
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    // Carried over from the earlier flat-quad version. draw() uses
    // glDrawArrays, so the EBO is currently unused.
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
  }

  void Cube::make_texture()
  {
    texture = loadTexture("./src/resources/textures/container.jpg", true,
                          GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST,
                          GL_NEAREST);
  }
} // namespace fivelabsengine
