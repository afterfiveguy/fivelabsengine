#ifndef SHADER_H
#define SHADER_H

#include <config.h>

namespace fivelabsengine
{
  class Shader
  {
  public:
    unsigned int ID;

    Shader(const char *vertexPath, const char *fragmentPath);
    ~Shader() { glDeleteProgram(ID); }
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
    Shader(Shader &&o) noexcept : ID(o.ID) { o.ID = 0; }

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &v) const;
  };
} // namespace fivelabsengine
#endif
