#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace GAB
{
// Optional adapter for an application's renderer. It owns GPU resources only;
// the engine owns the window, event polling, frame timing and presentation loop.
class GraphicsDevice
{
public:
  virtual ~GraphicsDevice() = default;
  virtual bool InitializeDevice(void* nativeWindow, uint32_t width, uint32_t height) = 0;
  virtual void ShutdownDevice() = 0;
  virtual bool BeginFrame(uint32_t width, uint32_t height) = 0;
  virtual bool EndFrame(bool vSync) = 0;
  virtual void Clear(const glm::vec4& color) = 0;
  virtual void DrawRectangle(const glm::vec2& center, const glm::vec2& size,
                             float rotationDegrees, const glm::vec4& color) = 0;
};
}
