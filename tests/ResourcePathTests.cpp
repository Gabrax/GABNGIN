#include <gabgl/Resources.h>

#include <filesystem>
#include <iostream>

int main()
{
  namespace fs = std::filesystem;
  const fs::path assets = fs::temp_directory_path() / "gabgl-application-assets";
  const fs::path imported = fs::temp_directory_path() / "imported.glb";
  GAB::Resources::SetAssetRoot(assets);
  const auto expected = (assets / "models/mesh.glb").lexically_normal().generic_string();
  bool passed = GAB::Resources::AssetPath("models/mesh.glb") == expected;
  passed &= GAB::Resources::AssetPath("res/models/mesh.glb") == expected;
  passed &= GAB::Resources::AssetPath("../res/models/mesh.glb") == expected;
  passed &= GAB::Resources::AssetPath(imported.generic_string()) == imported.generic_string();
  // The engine's own shader source must exist independently of game assets.
  passed &= fs::is_regular_file(GAB::Resources::ShaderPath("scene.slang"));
  passed &= fs::is_regular_file(GAB::Resources::ShaderPath("vulkan_basic.slang"));
  const auto shaders = GAB::Resources::ShaderPath("");
  GAB::Resources::SetAssetRoot(fs::temp_directory_path() / "another-application");
  passed &= GAB::Resources::ShaderPath("") == shaders;
  GAB::Resources::SetShaderRoot(assets / "custom-shaders");
  passed &= GAB::Resources::ShaderPath("custom.slang") == assets / "custom-shaders/custom.slang";
  std::cout << "Engine resources " << (passed ? "PASS" : "FAIL") << '\n';
  return passed ? 0 : 1;
}
