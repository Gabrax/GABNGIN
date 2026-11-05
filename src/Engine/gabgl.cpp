#include <gabgl/gabgl.h>
#include <gabgl/GraphicsDevice.h>
#include "OpenGLDevice.h"
#include <gabgl/Window.h>
#include <GLFW/glfw3.h>

#include <array>
#include <chrono>
#include <cmath>
#include <thread>

namespace GAB
{
namespace
{
  using Clock = std::chrono::steady_clock;
  std::unique_ptr<GraphicsDevice> s_DefaultDevice;
  GraphicsDevice* s_Device = nullptr;
  bool s_Drawing = false;
  bool s_VSync = false;
  uint32_t s_TargetFPS = 0;
  float s_FrameTime = 0.0f;
  Clock::time_point s_PreviousFrame, s_FrameStart;
  std::array<bool, GLFW_KEY_LAST + 1> s_KeysDown{}, s_KeysPressed{};
}

bool INIT(const InitConfig& config)
{
  const auto [width, height, title, api, configuredDevice, targetFPS, vSync] = config;
  GraphicsDevice* device = configuredDevice;
  if (s_Device || width == 0 || height == 0 || !title) return false;
  if (!device && api != GraphicsAPI::OpenGL) return false;
  if (!Window::Init(title, width, height, api)) return false;
  GraphicsAPIState::Set(api);
  if (!device)
  {
    s_DefaultDevice = CreateOpenGLDevice();
    device = s_DefaultDevice.get();
  }
  try
  {
    if (!device->InitializeDevice(Window::GetNativeHandle(), width, height))
    {
      device->ShutdownDevice();
      s_DefaultDevice.reset();
      Window::Terminate();
      return false;
    }
  }
  catch (...)
  {
    device->ShutdownDevice();
    s_DefaultDevice.reset();
    Window::Terminate();
    throw;
  }
  s_Device = device;
  s_Drawing = s_VSync = false;
  s_TargetFPS = targetFPS;
  s_FrameTime = 0.0f;
  s_KeysDown.fill(false);
  s_KeysPressed.fill(false);
  s_PreviousFrame = Clock::now();
  SetVSync(vSync);
  return true;
}

void TERMINATE()
{
  if (!s_Device) return;
  if (s_Drawing) EndDrawing();
  s_Device->ShutdownDevice();
  s_Device = nullptr;
  s_DefaultDevice.reset();
  Window::Terminate();
}

bool IsInitialized() { return s_Device != nullptr; }
bool WindowShouldClose() { return !s_Device || !Window::IsRunning(); }
void RequestClose() { if (s_Device) Window::RequestClose(); }

bool BeginDrawing()
{
  if (!s_Device || s_Drawing || !Window::IsRunning()) return false;
  Window::PollEvents();
  if (!Window::IsRunning()) return false;
  if (Window::IsMinimized())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
    s_PreviousFrame = Clock::now();
    return false;
  }
  s_FrameStart = Clock::now();
  s_FrameTime = std::chrono::duration<float>(s_FrameStart - s_PreviousFrame).count();
  s_PreviousFrame = s_FrameStart;
  for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
  {
    const bool down = glfwGetKey(Window::GetWindowPtr(), key) == GLFW_PRESS;
    s_KeysPressed[key] = down && !s_KeysDown[key];
    s_KeysDown[key] = down;
  }
  if (!s_Device->BeginFrame(Window::GetWidth(), Window::GetHeight()))
  {
    RequestClose();
    return false;
  }
  s_Drawing = true;
  return true;
}

void EndDrawing()
{
  if (!s_Drawing) return;
  s_Drawing = false;
  if (!s_Device->EndFrame(s_VSync)) RequestClose();
  if (!s_VSync && s_TargetFPS > 0)
    std::this_thread::sleep_until(s_FrameStart +
      std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(1.0 / s_TargetFPS)));
}

float GetFrameTime() { return s_FrameTime; }
void SetTargetFPS(uint32_t fps) { s_TargetFPS = fps; }
void SetVSync(bool enabled)
{
  s_VSync = enabled;
  if (s_Device) Window::SetVSync(enabled);
}
uint32_t GetScreenWidth() { return s_Device ? Window::GetWidth() : 0; }
uint32_t GetScreenHeight() { return s_Device ? Window::GetHeight() : 0; }
void SetWindowSize(uint32_t width, uint32_t height)
{
  if (s_Device && width > 0 && height > 0) Window::SetResolution(width, height);
}
void SetWindowMode(WindowMode mode, uint32_t width, uint32_t height)
{
  if (s_Device) Window::SetWindowMode(mode, width, height);
}
void SetWindowIcon(const char* path) { if (s_Device) Window::SetIcon(path); }

void ClearBackground(const glm::vec4& color) { if (s_Drawing) s_Device->Clear(color); }
void DrawRectangle(float x, float y, float width, float height, const glm::vec4& color)
{
  DrawRectanglePro({x + width * 0.5f, y + height * 0.5f}, {width, height}, 0.0f, color);
}
void DrawRectanglePro(const glm::vec2& center, const glm::vec2& size,
                      float rotation, const glm::vec4& color)
{
  if (s_Drawing) s_Device->DrawRectangle(center, size, rotation, color);
}
void DrawLine(const glm::vec2& start, const glm::vec2& end, float thickness,
              const glm::vec4& color)
{
  const glm::vec2 delta = end - start;
  DrawRectanglePro((start + end) * 0.5f, {glm::length(delta), thickness},
    glm::degrees(std::atan2(delta.y, delta.x)), color);
}

bool IsKeyDown(int key)
{
  return s_Device && key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST && s_KeysDown[key];
}
bool IsKeyPressed(int key)
{
  return s_Device && key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST && s_KeysPressed[key];
}
bool IsMouseButtonDown(int button)
{
  return s_Device && button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST &&
    glfwGetMouseButton(Window::GetWindowPtr(), button) == GLFW_PRESS;
}
glm::vec2 GetMousePosition()
{
  double x = 0.0, y = 0.0;
  if (s_Device) glfwGetCursorPos(Window::GetWindowPtr(), &x, &y);
  return {float(x), float(y)};
}
}
