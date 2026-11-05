#pragma once 

#include <cstdint>
#include <functional>
#include <string>

struct GLFWwindow;
#include <gabgl/GraphicsAPI.h>
#include <gabgl/WindowMode.h>
#include <gabgl/Event.h>
#include <gabgl/EngineEvent.h>

namespace GAB
{
struct Window 
{
	using EventCallbackFn = std::function<void(Event&)>;

  static bool Init(const std::string& windowTitle, uint32_t windowWidth, uint32_t windoHeight,
                   GraphicsAPI graphicsAPI = GraphicsAPI::OpenGL);
  static void Terminate();
	static void Update();
	static void PollEvents();
	static void Present();
	static uint32_t GetWidth();
	static uint32_t GetHeight();
	static GLFWwindow* GetWindowPtr();
	static void* GetNativeHandle();
	static void SetEventCallback(const EventCallbackFn& callback);
  static void SetResizeCallback(std::function<void(uint32_t, uint32_t)> callback);
  static void SetIcon(const char* path);
	static bool isClosed();
	static void SetVSync(bool enabled);
	static bool IsVSync();
  static void Maximize(bool maximize);
  static void SetResolution(uint32_t width, uint32_t height);
	static void SetFullscreen(bool full);
	static void SetWindowMode(WindowMode mode, uint32_t width, uint32_t height);
	static void RequestClose();
	static void SetResizable(bool enable);
  static void CenterWindowPos();
  static void SetCursorVisible(bool enable);
  static bool IsRunning();
  static bool IsMinimized();

private:

  static void SetWindowIcon(const char* iconpath, GLFWwindow* window);
  static void OnEvent(Event& e);
  static bool OnWindowClose(WindowCloseEvent& e);
  static bool OnWindowResize(WindowResizeEvent& e);
};
} // namespace GAB
