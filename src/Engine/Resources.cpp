#include <gabgl/Resources.h>

namespace GAB
{
namespace
{
  std::filesystem::path s_AssetRoot = std::filesystem::absolute("res");
  std::filesystem::path s_ShaderRoot = GABGL_SHADER_DIR;
}

void Resources::SetAssetRoot(const std::filesystem::path& root)
{
  s_AssetRoot = std::filesystem::absolute(root).lexically_normal();
}

std::filesystem::path Resources::GetAssetRoot() { return s_AssetRoot; }

std::string Resources::AssetPath(std::string_view path)
{
  // Preserve compatibility with older scene files and absolute imported assets.
  if (path.starts_with("../res/")) path.remove_prefix(7);
  else if (path.starts_with("res/")) path.remove_prefix(4);
  return (s_AssetRoot / std::filesystem::path(path)).lexically_normal().generic_string();
}

void Resources::SetShaderRoot(const std::filesystem::path& root)
{
  s_ShaderRoot = std::filesystem::absolute(root).lexically_normal();
}

std::filesystem::path Resources::ShaderPath(std::string_view path)
{
  return (s_ShaderRoot / std::filesystem::path(path)).lexically_normal();
}
}
