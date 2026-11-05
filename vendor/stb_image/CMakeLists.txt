cmake_minimum_required(VERSION 3.5)
project(stb_image)

cmake_policy(SET CMP0069 NEW)
add_library(stb_image STATIC)
target_sources(stb_image PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.cpp")
target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")



