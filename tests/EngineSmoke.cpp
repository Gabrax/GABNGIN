// Desktop/GPU regression, requiring only GAB::engine and no game resources.
#include <gabgl/gabgl.h>
#include <gabgl/GraphicsDevice.h>
#include <glad/glad.h>
#include <array>
#include <cmath>
#include <iostream>

namespace
{
bool Near(const std::array<unsigned char, 4>& pixel, int r, int g, int b)
{
  return std::abs(int(pixel[0]) - r) <= 2 && std::abs(int(pixel[1]) - g) <= 2 &&
    std::abs(int(pixel[2]) - b) <= 2;
}

// Checks that an application-supplied device participates in the same loop.
struct Device final : GAB::GraphicsDevice
{
  int initialized = 0, shutDown = 0, frames = 0, presents = 0, clears = 0, rectangles = 0;
  bool failInitialization = false, failFrame = false;
  bool InitializeDevice(void*, uint32_t, uint32_t) override
  { ++initialized; return !failInitialization; }
  void ShutdownDevice() override { ++shutDown; }
  bool BeginFrame(uint32_t, uint32_t) override { ++frames; return !failFrame; }
  bool EndFrame(bool) override { ++presents; return true; }
  void Clear(const glm::vec4&) override { ++clears; }
  void DrawRectangle(const glm::vec2&, const glm::vec2&, float, const glm::vec4&) override
  { ++rectangles; }
};
}

int main()
{
  bool passed = true;
  // Reopening catches stale GLFW lifecycle state. Readback checks coordinates,
  // colors and resize behavior in the actual built-in renderer.
  for (int run = 0; run < 2; ++run)
  {
    const bool initialized = run == 0 ? GAB::INIT()
      : GAB::INIT({.Width = 320, .Height = 240, .Title = "GABGL engine smoke"});
    if (!initialized) return 1;
    passed &= GAB::IsInitialized() && !GAB::INIT();
    passed &= GAB::Window::GetWidth() == GAB::GetScreenWidth();
    for (int frame = 0; frame < 3; ++frame)
    {
      if (frame == 1) GAB::SetWindowSize(321, 241);
      if (!GAB::BeginDrawing()) { passed = false; break; }
      GAB::ClearBackground({0, 0, 1, 1});
      GAB::DrawRectangle(10, 10, 50, 40, {1, 0, 0, 1});
      std::array<unsigned char, 4> inside{}, outside{};
      glReadPixels(20, GAB::GetScreenHeight() - 21, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, inside.data());
      glReadPixels(100, 100, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, outside.data());
      passed &= Near(inside, 255, 0, 0) && Near(outside, 0, 0, 255);
      passed &= glGetError() == GL_NO_ERROR;
      GAB::EndDrawing();
    }
    GAB::TERMINATE();
    GAB::TERMINATE();
    passed &= GAB::WindowShouldClose() && !GAB::IsInitialized();
  }

  Device failedDevice;
  failedDevice.failInitialization = true;
  passed &= !GAB::INIT({.API = GAB::GraphicsAPI::DirectX12, .Device = &failedDevice});
  passed &= !GAB::IsInitialized() && failedDevice.initialized == 1 && failedDevice.shutDown == 1;

  Device device;
  if (!GAB::INIT({.Width = 320, .Height = 240, .Title = "GABGL device smoke",
                   .API = GAB::GraphicsAPI::DirectX12, .Device = &device})) return 2;
  if (!GAB::BeginDrawing()) return 3;
  GAB::ClearBackground({0, 0, 0, 1});
  GAB::DrawRectangle(0, 0, 10, 10, {1, 1, 1, 1});
  GAB::EndDrawing();
  device.failFrame = true;
  passed &= !GAB::BeginDrawing();
  passed &= GAB::WindowShouldClose();
  GAB::TERMINATE();
  passed &= device.initialized == 1 && device.shutDown == 1 && device.frames == 2 &&
    device.presents == 1 && device.clears == 1 && device.rectangles == 1;
  std::cout << "Engine smoke " << (passed ? "PASS" : "FAIL") << '\n';
  return passed ? 0 : 4;
}
