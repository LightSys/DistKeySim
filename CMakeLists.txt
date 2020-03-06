cmake_minimum_required(VERSION 2.8)

# Project name
project(ADAK_Keying)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 2)
set(PROJECT_VERSION_PATCH 0)

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Build configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindProtobuf REQUIRED)
include(CTest)

# Executable
add_subdirectory(src)

# Unit tests
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
