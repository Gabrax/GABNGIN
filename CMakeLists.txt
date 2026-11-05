cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.16)
project(gabgl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC AND NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
  # The bundled PhysX libraries use the dynamic release CRT.
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

set(GABGL_STANDALONE OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(GABGL_STANDALONE ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  include(CTest)
endif()
option(GABGL_BUILD_EXAMPLES "Build engine examples" ${GABGL_STANDALONE})
option(GABGL_BUILD_TESTS "Build engine integration tests" ${GABGL_STANDALONE})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw-3.3.2)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/stb_image)
add_subdirectory(vendor/glm)
include(cmake/Dependencies.cmake)

# All reusable rendering, scene, editor, audio and physics code belongs to GABGL.
file(GLOB GABGL_RUNTIME_SOURCES CONFIGURE_DEPENDS "src/Runtime/*.cpp")
add_library(gabgl_engine STATIC
  src/Engine/gabgl.cpp
  src/Engine/OpenGLDevice.cpp
  src/Engine/Resources.cpp
  src/Platform/Window.cpp
  src/Input/UserInput.cpp
  ${GABGL_RUNTIME_SOURCES}
)
add_library(GAB::engine ALIAS gabgl_engine)
target_compile_features(gabgl_engine PUBLIC cxx_std_20)
set_target_properties(gabgl_engine PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(gabgl_engine
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/Runtime"
)
target_compile_definitions(gabgl_engine
  PUBLIC GLFW_INCLUDE_NONE=1
  PRIVATE GABGL_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res/shaders"
)
if(MSVC)
  target_compile_definitions(gabgl_engine PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()
target_link_libraries(gabgl_engine
  PUBLIC glm glfw glad stb_image imgui assimp meshoptimizer JSONparser PhysX
    SndFile::sndfile OpenAL::OpenAL freetype gabdebug::gabdebug
  PRIVATE ${GABGL_SLANG_TARGET}
)
if(GABGL_ENABLE_DX12)
  if(NOT WIN32)
    message(FATAL_ERROR "GABGL_ENABLE_DX12 is only supported on Windows")
  endif()
  target_compile_definitions(gabgl_engine PRIVATE GABGL_ENABLE_DX12=1)
  target_link_libraries(gabgl_engine PRIVATE d3d12 dxgi dxguid)
endif()
if(GABGL_ENABLE_VULKAN)
  target_compile_definitions(gabgl_engine PRIVATE GABGL_ENABLE_VULKAN=1)
  target_link_libraries(gabgl_engine PRIVATE Vulkan::Vulkan)
endif()

# Target properties survive the FetchContent directory boundary. Applications
# can deploy engine DLLs without knowing where the engine's SDKs were found.
if(WIN32)
  find_file(GABGL_SLANG_DLL NAMES slang-compiler.dll slang.dll
    HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{SLANG_ROOT}/bin")
  if(GABGL_SLANG_DLL)
    set_property(TARGET gabgl_engine PROPERTY GABGL_RUNTIME_FILES "${GABGL_SLANG_DLL}")
  endif()
endif()
function(gabgl_deploy_runtime application)
  get_target_property(runtime_files gabgl_engine GABGL_RUNTIME_FILES)
  if(runtime_files)
    add_custom_command(TARGET ${application} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${runtime_files} "$<TARGET_FILE_DIR:${application}>"
      COMMAND_EXPAND_LISTS VERBATIM
    )
  endif()
endfunction()

if(GABGL_BUILD_EXAMPLES)
  add_executable(gabgl_basic_window examples/basic_window.cpp)
  target_link_libraries(gabgl_basic_window PRIVATE GAB::engine)
  gabgl_deploy_runtime(gabgl_basic_window)
endif()

if(GABGL_BUILD_TESTS AND BUILD_TESTING)
  add_executable(engine_smoke EXCLUDE_FROM_ALL tests/EngineSmoke.cpp)
  target_link_libraries(engine_smoke PRIVATE GAB::engine)
  gabgl_deploy_runtime(engine_smoke)

  add_executable(engine_resource_tests tests/ResourcePathTests.cpp)
  target_link_libraries(engine_resource_tests PRIVATE GAB::engine)
  add_test(NAME engine_resources COMMAND engine_resource_tests)
  gabgl_deploy_runtime(engine_resource_tests)

  if(GABGL_ENABLE_DX12)
    add_executable(dx12_instance_tests tests/DX12InstanceTests.cpp)
    target_link_libraries(dx12_instance_tests PRIVATE GAB::engine d3dcompiler)
    add_test(NAME dx12_instances COMMAND dx12_instance_tests)
    set_tests_properties(dx12_instances PROPERTIES TIMEOUT 60)
    gabgl_deploy_runtime(dx12_instance_tests)
  endif()
  if(GABGL_ENABLE_VULKAN)
    add_executable(vulkan_bootstrap_smoke EXCLUDE_FROM_ALL tests/VulkanBootstrapSmoke.cpp)
    target_link_libraries(vulkan_bootstrap_smoke PRIVATE GAB::engine)
    gabgl_deploy_runtime(vulkan_bootstrap_smoke)
  endif()
endif()
