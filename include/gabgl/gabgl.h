#pragma once

#include "GraphicsAPI.h"
#include "WindowMode.h"
#include "KeyCodes.h"
#include "Window.h"
#include "UserInput.h"
#include <cstdint>
#include <glm/glm.hpp>

namespace GAB
{
class GraphicsDevice;

struct InitConfig
{
  uint32_t Width = 1280;
  uint32_t Height = 720;
  const char* Title = "GABGL";
  GraphicsAPI API = GraphicsAPI::OpenGL;
  GraphicsDevice* Device = nullptr;
  uint32_t TargetFPS = 60;
  bool VSync = false;
};

// One call owns window/device creation, input state and frame timing. A custom
// device must outlive TERMINATE(). The defaults need no game assets or setup.
bool INIT(const InitConfig& config = {});
void TERMINATE();
bool IsInitialized();
bool WindowShouldClose();
void RequestClose();

// BeginDrawing polls events. Skip drawing when it returns false (minimized or
// closing). Every successful BeginDrawing must be paired with EndDrawing.
bool BeginDrawing();
void EndDrawing();
float GetFrameTime();
void SetTargetFPS(uint32_t fps); // 0 = unlimited
void SetVSync(bool enabled);
uint32_t GetScreenWidth();
uint32_t GetScreenHeight();
void SetWindowSize(uint32_t width, uint32_t height);
void SetWindowMode(WindowMode mode, uint32_t width, uint32_t height);
void SetWindowIcon(const char* path);

// Screen coordinates start at the top left; colors are linear RGBA in [0, 1].
void ClearBackground(const glm::vec4& color);
void DrawRectangle(float x, float y, float width, float height, const glm::vec4& color);
void DrawRectanglePro(const glm::vec2& center, const glm::vec2& size,
                      float rotationDegrees, const glm::vec4& color);
void DrawLine(const glm::vec2& start, const glm::vec2& end, float thickness,
              const glm::vec4& color);

// Use Key::A, Key::Escape, Mouse::ButtonLeft, etc.; no GLFW header is required.
bool IsKeyDown(int key);
bool IsKeyPressed(int key);
bool IsMouseButtonDown(int button);
glm::vec2 GetMousePosition();
}
