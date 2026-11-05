# GABGL

A C++20 game engine with OpenGL, DirectX 12 and Vulkan backends. GABGL owns the
rendering pipeline, shadows, bloom, PS1 effects, particles, editor, scene loading
and persistence, model/animation management, audio, fonts, physics, window and input.
[GAB_THE_GAME](https://github.com/Gabrax/GAB_THE_GAME) is a separate application
that supplies gameplay, concrete scenes and its own assets.

Public headers live in `include/gabgl`; implementations live in `src/Engine`,
`src/Runtime`, `src/Platform` and `src/Input`. All public names use `GAB::`.
The complete engine is exposed by one CMake target, `GAB::engine`.

On Windows x64, use a Visual Studio Developer PowerShell with CMake and Ninja.
Install a Vulkan SDK containing Slang, or set `SLANG_ROOT` to a standalone Slang
SDK. The bundled PhysX libraries target Windows x64. OpenGL is always compiled;
`GABGL_ENABLE_DX12` defaults to on on Windows and `GABGL_ENABLE_VULKAN` defaults
to on when its SDK is available. These options belong to the engine.

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Applications consume the engine with FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(gabgl
  GIT_REPOSITORY https://github.com/Gabrax/GABGL.git
  GIT_TAG main # Pin a published engine revision for release builds.
)
FetchContent_MakeAvailable(gabgl)
add_executable(my_game main.cpp)
target_link_libraries(my_game PRIVATE GAB::engine)
gabgl_deploy_runtime(my_game)
```

Examples and engine tests default to off when embedded. For unpublished engine
changes, configure the application with
`-DFETCHCONTENT_SOURCE_DIR_GABGL=C:/path/to/GABGL`. CMake fetches the engine's
external dependencies and provides its vendored dependencies transitively.
`gabgl_deploy_runtime` copies the required Slang runtime beside an executable.

A minimal 2D OpenGL application uses the window/frame API:

```cpp
#include <gabgl/gabgl.h>

int main()
{
    if (!GAB::INIT()) return 1;
    while (!GAB::WindowShouldClose())
    {
        if (!GAB::BeginDrawing()) continue;
        GAB::ClearBackground({0.04f, 0.06f, 0.1f, 1.0f});
        GAB::DrawRectangle(100, 100, 80, 80, {0.2f, 0.7f, 1.0f, 1.0f});
        GAB::EndDrawing();
    }
    GAB::TERMINATE();
}
```

`gabgl_basic_window` is a complete example. The full scene pipeline selects an
engine backend through `GAB::RenderBackend::Select(api)` and passes
`&GAB::RenderBackend::Get()` as `GAB::InitConfig::Device`. The application composes
its service startup and teardown, as shown by the game's `GameBootstrap.cpp`.
The built-in scene renderers for all three APIs are compiled in the engine.
Applications may also provide another implementation of `GAB::GraphicsDevice`.

Engine shaders live in `res/shaders`; the engine resolves them independently of
application assets and the working directory. A packaged application can override
their location with `GAB::Resources::SetShaderRoot`. The application configures
its own assets before loading scenes:

```cpp
GAB::Resources::SetAssetRoot("C:/path/to/my-game/res");
GAB::SceneManager::SetSceneFile("scenes/scene.json");
// After RenderSystem::Initialize(), load application fonts explicitly:
GAB::FontManager::LoadFont(GAB::Resources::AssetPath("fonts/ui.ttf").c_str());
```

The first loaded font becomes the default used by loading/debug/editor UI;
`SetDefaultFont` can choose another loaded font. The camera follows the scene
entity marked `player`, without assuming a particular game's model name.
Scene factories and gameplay interaction callbacks are supplied by the application.
Relative scene assets, legacy `../res/` paths and absolute imported paths work.

CTest runs resource-boundary checks and the D3D12 WARP shader regression when
DX12 is enabled. Asset-free desktop integration targets are opt-in:

```powershell
cmake --build build --target engine_smoke vulkan_bootstrap_smoke
.\build\engine_smoke.exe
.\build\vulkan_bootstrap_smoke.exe
```

The Vulkan target requires `GABGL_ENABLE_VULKAN=ON`. Complete scene-rendering
integration tests using the game's assets belong to GAB_THE_GAME.
