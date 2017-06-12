// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Window
// -----------------------------------------------------------------------------

#include "window.hpp"
#include <utility>
#include <cstring>
#include <iostream>
#include <cassert>
#include <exception>

const char* vsource = R"(
#version 330
out vec2 v_texCoord;
void main()
{
  v_texCoord = vec2(float(gl_VertexID / 2) * 2.0, float(gl_VertexID % 2) * 2.0);
  gl_Position = vec4((float(gl_VertexID / 2) * 4.0) - 1.0,
                     (float(gl_VertexID % 2) * 4.0) - 1.0,
                     0.0,
                     1.0);
}
)";

const char* fsource = R"(
#version 330
uniform sampler2D image;
in vec2 v_texCoord;

out vec4 FragColor;

void main()
{
  vec4 color = texture(image, v_texCoord).rgba;
  FragColor = color;
}
)";

namespace {
void errorcb(int error, const char* desc)
{
  std::cerr << "GLFW error " << error << ": " << desc << std::endl;
}
}

Window::Window(glm::ivec2 const& windowsize)
  : m_window{nullptr}
  , m_windowSize(windowsize)
  , m_framebufferSize{windowsize}
  , m_title("Fensterchen")
{
  if (!glfwInit()) {
    throw "Could not init glfw";
  }
  glfwSetErrorCallback(errorcb);
  //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

  glfwWindowHint(GLFW_RESIZABLE, 0);

  m_window = glfwCreateWindow(windowsize.x, windowsize.y, m_title.c_str(), nullptr, nullptr);
  if (!m_window) {
    glfwTerminate();
    throw "Could not create window";
  }

  glfwMakeContextCurrent(m_window);

  //  the pollable state of a mouse button will remain GLFW_PRESS until the
  //  state of that button is polled with glfwGetMouseButton. Once it has been
  //  polled, if a mouse button release event had been processed in the
  //  meantime, the state will reset to GLFW_RELEASE, otherwise it will remain
  //  GLFW_PRESS.
  glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, 1);

  glfwSetWindowUserPointer(m_window, this);
  assert(m_window != nullptr);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
      throw "Could not init glew";
  }
  glGetError();

  // Begin Frame
  glfwGetFramebufferSize(m_window, &m_framebufferSize.x, &m_framebufferSize.y);

  // Calculate pixel ration for hi-dpi devices.
  float pxRatio = float(m_framebufferSize.x) / float(m_windowSize.x);
  glViewport(0, 0, m_framebufferSize.x, m_framebufferSize.y);
  //glClearColor(1.0f,1.0f,1.0f,1.0f);
  glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  //nvgBeginFrame(m_nvgContext, m_windowSize.x, m_windowSize.y, pxRatio);
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  auto vsh = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vsh, 1, &vsource ,0);
  glCompileShader(vsh);
  GLint result = GL_FALSE;
  glGetShaderiv(vsh, GL_COMPILE_STATUS, &result);
  if (result != GL_TRUE) {
    std::string log(1000, '*');
    glGetShaderInfoLog(vsh, 1000, 0, (GLchar*)log.data());
    std::cout << "Error vertex shader: " << log;
    throw "Error glCompileShader vertex";
  }
  auto fsh = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fsh, 1, &fsource ,0);
  glCompileShader(fsh);
  glGetShaderiv(fsh, GL_COMPILE_STATUS, &result);
  if (result != GL_TRUE) {
    std::string log(1000, '*');
    glGetShaderInfoLog(fsh, 1000, 0, (GLchar*)log.data());
    std::cout << "Error fragment shader: " << log;
    throw "Error glCompileShader fragment";
  }
  m_program = glCreateProgram();
  glAttachShader(m_program, vsh);
  glAttachShader(m_program, fsh);
  glLinkProgram(m_program);
  glUseProgram(m_program);
  GLint image_loc = glGetUniformLocation(m_program, "image");
  if (image_loc != -1) {
    glUniform1i(image_loc, 0);
  }
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, m_windowSize.x, m_windowSize.y);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Window::~Window()
{
  glDeleteVertexArrays(1, &m_vao);
  glfwTerminate();
}

int Window::get_key(int key) const
{
  return glfwGetKey(m_window, key);
}

int Window::get_mouse_button(int button) const
{
  return glfwGetMouseButton(m_window, button);
}

bool Window::should_close() const
{
  return glfwWindowShouldClose(m_window);
}

glm::vec2 Window::mouse_position() const
{
  double xpos=0.0;
  double ypos=0.0f;
  glfwGetCursorPos(m_window, &xpos, &ypos);
  return glm::vec2(float(xpos), float(ypos));
}

void Window::close()
{
  glfwSetWindowShouldClose(m_window, GL_TRUE);
}

void Window::update()
{
  // End Frame
  glfwSwapBuffers(m_window);
  glfwPollEvents();

  // Begin Frame
  glViewport(0, 0, m_framebufferSize.x, m_framebufferSize.y);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Calculate pixel ration for hi-dpi devices.
  float pxRatio = float(m_framebufferSize.x) / float(m_windowSize.x);
  //nvgBeginFrame(m_nvgContext, m_windowSize.x, m_windowSize.y, pxRatio);
}

glm::ivec2 Window::window_size() const
{
  glm::ivec2 size(0);
  glfwGetFramebufferSize(m_window, &size.x, &size.y);
  return size;
}

float Window::get_time() const
{
  return float(glfwGetTime());
}

void Window::show(std::vector<Color> const& colorbuffer)
{
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_windowSize.x, m_windowSize.y, GL_RGB, GL_FLOAT, colorbuffer.data());
  glBindVertexArray(m_vao);
  glUseProgram(m_program);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
  update();
}
