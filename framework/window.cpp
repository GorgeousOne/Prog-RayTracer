// -----------------------------------------------------------------------------
// Copyright  : (C) 2014 Andreas-C. Bernstein
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
}

Window::~Window()
{
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
