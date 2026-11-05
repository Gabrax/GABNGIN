#include "OpenGLDevice.h"
#include <gabgl/Window.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>

namespace GAB
{
namespace
{
GLuint Compile(GLenum type, const char* source)
{
  const GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  GLint success = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_TRUE) return shader;
  char message[2048]{};
  glGetShaderInfoLog(shader, sizeof(message), nullptr, message);
  std::fprintf(stderr, "GABGL shader: %s\n", message);
  glDeleteShader(shader);
  return 0;
}

class OpenGLDevice final : public GraphicsDevice
{
public:
  bool InitializeDevice(void*, uint32_t, uint32_t) override
  {
    constexpr const char* vertexSource = R"(
      #version 330 core
      layout(location = 0) in vec2 position;
      uniform mat4 transform;
      void main() { gl_Position = transform * vec4(position, 0.0, 1.0); }
    )";
    constexpr const char* fragmentSource = R"(
      #version 330 core
      uniform vec4 color;
      out vec4 outputColor;
      void main() { outputColor = color; }
    )";
    const GLuint vertex = Compile(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragment = Compile(GL_FRAGMENT_SHADER, fragmentSource);
    if (!vertex || !fragment)
    {
      if (vertex) glDeleteShader(vertex);
      if (fragment) glDeleteShader(fragment);
      return false;
    }
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertex);
    glAttachShader(m_Program, fragment);
    glLinkProgram(m_Program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    GLint success = GL_FALSE;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
      ShutdownDevice();
      return false;
    }
    constexpr float vertices[] = {
      -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
      -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    m_Transform = glGetUniformLocation(m_Program, "transform");
    m_Color = glGetUniformLocation(m_Program, "color");
    return true;
  }

  void ShutdownDevice() override
  {
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_Program) glDeleteProgram(m_Program);
    m_VBO = m_VAO = m_Program = 0;
  }

  bool BeginFrame(uint32_t width, uint32_t height) override
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_Projection = glm::ortho(0.0f, float(width), float(height), 0.0f);
    return true;
  }

  bool EndFrame(bool vSync) override
  {
    if (Window::IsVSync() != vSync) Window::SetVSync(vSync);
    Window::Present();
    return true;
  }

  void Clear(const glm::vec4& color) override
  {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void DrawRectangle(const glm::vec2& center, const glm::vec2& size,
                     float rotation, const glm::vec4& color) override
  {
    const glm::mat4 transform = m_Projection *
      glm::translate(glm::mat4(1.0f), glm::vec3(center, 0.0f)) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
      glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
    glUseProgram(m_Program);
    glUniformMatrix4fv(m_Transform, 1, GL_FALSE, glm::value_ptr(transform));
    glUniform4fv(m_Color, 1, glm::value_ptr(color));
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

private:
  GLuint m_Program = 0, m_VAO = 0, m_VBO = 0;
  GLint m_Transform = -1, m_Color = -1;
  glm::mat4 m_Projection{1.0f};
};
}

std::unique_ptr<GraphicsDevice> CreateOpenGLDevice()
{
  return std::make_unique<OpenGLDevice>();
}
}
