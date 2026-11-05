#include <gabgl/RenderBackend.h>
#include <gabgl/Window.h>

#include <cstdint>
#include <iostream>

int main()
{
  constexpr uint32_t InitialWidth = 640;
  constexpr uint32_t InitialHeight = 360;
  if (!GAB::RenderBackend::Select(GAB::GraphicsAPI::Vulkan)) return 1;
  GAB::Window::Init("GABGL Vulkan bootstrap smoke", InitialWidth, InitialHeight, GAB::GraphicsAPI::Vulkan);
  auto& backend = GAB::RenderBackend::Get();
  if (!backend.InitializeDevice(nullptr, InitialWidth, InitialHeight))
  {
    GAB::Window::Terminate();
    return 2;
  }

  bool passed = true;
  for (uint32_t frame = 0; frame < 24 && GAB::Window::IsRunning(); ++frame)
  {
    GAB::Window::PollEvents();
    if (frame == 8) GAB::Window::SetResolution(641, 359);
    if (frame == 16) GAB::Window::SetResolution(InitialWidth, InitialHeight);
    if (!backend.BeginFrame(GAB::Window::GetWidth(), GAB::Window::GetHeight()) ||
        !backend.EndFrame(frame >= 12))
    {
      passed = false;
      break;
    }
  }
  backend.ShutdownDevice();
  GAB::Window::Terminate();
  std::cout << "Vulkan bootstrap smoke " << (passed ? "PASS" : "FAIL") << '\n';
  return passed ? 0 : 3;
}
