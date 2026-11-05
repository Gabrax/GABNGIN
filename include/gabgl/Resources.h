#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace GAB
{
// Engine shaders and application assets have independent roots. Applications
// configure their own asset directory before loading scenes or other resources.
struct Resources
{
  static void SetAssetRoot(const std::filesystem::path& root);
  static std::filesystem::path GetAssetRoot();
  static std::string AssetPath(std::string_view path);
  static void SetShaderRoot(const std::filesystem::path& root);
  static std::filesystem::path ShaderPath(std::string_view path);
};
}
