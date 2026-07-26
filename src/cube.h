#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace fivelabsengine
{
  class Cube
  {
  public:
    Cube(glm::vec3 position, glm::vec3 size,
         glm::vec3 eulers = glm::vec3(0.0f));
    ~Cube();

    // A Cube owns GL handles (VAO/VBO/EBO/texture). Copying it would let two
    // Cubes delete the same handles, so we forbid copies and allow moves --
    // moving is what lets cubes live in a std::vector.
    Cube(const Cube &) = delete;
    Cube &operator=(const Cube &) = delete;
    Cube(Cube &&o) noexcept;
    Cube &operator=(Cube &&o) noexcept;

    void update(float dt);
    // Uploads this cube's model matrix and binds its texture to unit 0, then
    // draws. `shader` is the program ID, which must already be in use().
    void draw(unsigned int shader);

  private:
    glm::vec3 position, size, eulers;
    unsigned int VAO, VBO, EBO, texture;

    void make_texture();
    void make_mesh();
    void release();

    // static: builds a texture from a file without touching any member state
    static unsigned int loadTexture(const char *path, bool flipVertically = true,
                                    GLenum wrapS = GL_REPEAT,
                                    GLenum wrapT = GL_REPEAT,
                                    GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
                                    GLenum magFilter = GL_LINEAR);
  };
} // namespace fivelabsengine
