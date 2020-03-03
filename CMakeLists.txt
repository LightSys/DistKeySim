cmake_minimum_required(VERSION 2.8)

# Project name
project(ADAK_Keying)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 2)
set(PROJECT_VERSION_PATCH 0)

# Build configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(Protobuf REQUIRED)

# Executable
add_subdirectory(src)
install(TARGETS adak DESTINATION ${PROJECT_BINARY_DIR})

# Unit tests
enable_testing()
add_subdirectory(tests)