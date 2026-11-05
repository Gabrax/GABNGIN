include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Do not update populated dependencies during configure")

option(GABGL_ENABLE_DX12 "Build the optional DirectX 12 backend" ${WIN32})
find_package(Vulkan QUIET)
option(GABGL_ENABLE_VULKAN "Build the Vulkan backend" ${Vulkan_FOUND})

if(GABGL_ENABLE_VULKAN AND NOT Vulkan_FOUND)
  find_package(Vulkan REQUIRED)
endif()

# Slang is the single source-language compiler for the graphics backends.
# The Vulkan SDK ships a complete Slang SDK; standalone Slang installations are
# also supported through CMAKE_PREFIX_PATH or SLANG_ROOT.
find_package(slang CONFIG QUIET)
if(TARGET slang::slang)
  set(GABGL_SLANG_TARGET slang::slang)
else()
  find_path(GABGL_SLANG_INCLUDE_DIR
    NAMES slang.h
    HINTS "$ENV{VULKAN_SDK}/Include/slang" "$ENV{SLANG_ROOT}/include"
    PATH_SUFFIXES slang include/slang include
  )
  find_library(GABGL_SLANG_LIBRARY
    NAMES slang-compiler slang
    HINTS "$ENV{VULKAN_SDK}/Lib" "$ENV{SLANG_ROOT}/lib"
  )
  if(NOT GABGL_SLANG_INCLUDE_DIR OR NOT GABGL_SLANG_LIBRARY)
    message(FATAL_ERROR "Slang SDK not found. Install Slang or a Vulkan SDK containing Slang, then set VULKAN_SDK or SLANG_ROOT.")
  endif()
  add_library(gabgl_slang UNKNOWN IMPORTED)
  set_target_properties(gabgl_slang PROPERTIES
    IMPORTED_LOCATION "${GABGL_SLANG_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${GABGL_SLANG_INCLUDE_DIR}"
  )
  set(GABGL_SLANG_TARGET gabgl_slang)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries only" FORCE)
# Assimp 
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG f3e5f003b36b11d33b2abff40797fcac73b07892
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp tests" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "Disable Assimp documentation" FORCE)

# Meshoptimizer
FetchContent_Declare(
    meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG v0.18
)

# libsndfile
FetchContent_Declare(
    libsndfile
    GIT_REPOSITORY https://github.com/libsndfile/libsndfile
    GIT_TAG 1.2.2
)

# OpenAL Soft
FetchContent_Declare(
    openal-soft
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG 1.23.1 
)

# GABDEBUG supplies the engine logger and the CPU/GPU profiler. Pin the exact
# 0.3.0 revision so diagnostics do not change underneath reproducible builds.
FetchContent_Declare(
    gabdebug
    GIT_REPOSITORY https://github.com/Gabrax/GABDEBUG.git
    GIT_TAG d724c84a5690e507c7d8f7cd32298a6af759fe4a
)
set(GABDEBUG_BUILD_TESTS OFF CACHE BOOL "Disable dependency tests" FORCE)
set(_gabgl_gabdebug_gpu_apis OPENGL)
if(GABGL_ENABLE_VULKAN)
    list(APPEND _gabgl_gabdebug_gpu_apis VULKAN)
endif()
if(GABGL_ENABLE_DX12 AND WIN32)
    list(APPEND _gabgl_gabdebug_gpu_apis D3D12)
endif()
# Clear the deprecated single-backend cache entry left by older build trees.
unset(GABDEBUG_GPU_API CACHE)
set(GABDEBUG_GPU_APIS "${_gabgl_gabdebug_gpu_apis}" CACHE STRING
    "Build GABDEBUG profiler adapters used by GABGL" FORCE)

set(LIBTYPE STATIC CACHE STRING "Build static OpenAL Soft" FORCE)
# Freetype
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG VER-2-13-2 
)

# Dependencies must not register their own tests or sample programs in this
# application's default build/CTest run. Keep BUILD_TESTING local to this scope.
function(gabgl_fetch_engine_dependencies)
  set(BUILD_TESTING OFF)
  set(BUILD_PROGRAMS OFF)
  set(BUILD_EXAMPLES OFF)
  set(ALSOFT_UTILS OFF)
  set(ALSOFT_EXAMPLES OFF)
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  FetchContent_MakeAvailable(assimp meshoptimizer libsndfile openal-soft freetype gabdebug)
endfunction()
gabgl_fetch_engine_dependencies()

# Keep GABDEBUG's short LOG_*/PROFILER_* aliases out of engine headers.
target_compile_definitions(gabdebug PUBLIC
  GABLOG_UNSTRIP_PREFIX=1
  GABPROFILER_UNSTRIP_PREFIX=1
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-docking" "${CMAKE_CURRENT_BINARY_DIR}/vendor/imgui")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vendor/nlohmann-json" "${CMAKE_CURRENT_BINARY_DIR}/vendor/json")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vendor/physx" "${CMAKE_CURRENT_BINARY_DIR}/vendor/physx")

