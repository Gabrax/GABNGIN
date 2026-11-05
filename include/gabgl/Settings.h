#pragma once

#include <cstdint>
#include <filesystem>
#include <gabgl/GraphicsAPI.h>
#include <gabgl/WindowMode.h>

namespace GAB
{
enum class GraphicsQuality : uint32_t
{
  Off = 0,
  Low = 1,
  Medium = 2,
  High = 3
};

struct Settings
{
  static void Init();

  static void Save();
  static void ReloadIfChanged();
  static void ForceReload();

  static uint32_t GetWindowWidth();
  static uint32_t GetWindowHeight();
  static void SetResolution(uint32_t width, uint32_t height);
  static WindowMode GetWindowMode();
  static void SetWindowMode(WindowMode mode);
  static uint32_t GetFPSLimit();
  static void SetFPSLimit(uint32_t fps);
  static bool GetVSync();
  static void SetVSync(bool enabled);
  static float GetMusicVolume();
  static void SetMusicVolume(float volume);
  static float GetSFXVolume();
  static void SetSFXVolume(float volume);
  static GraphicsQuality GetShadowQuality();
  static void SetShadowQuality(GraphicsQuality quality);
  static GraphicsQuality GetBloomQuality();
  static void SetBloomQuality(GraphicsQuality quality);
  static GraphicsAPI GetGraphicsAPI();
  static void SetGraphicsAPI(GraphicsAPI api);

private:
  static void Load();
  static void SetDefaults();
};
} // namespace GAB
