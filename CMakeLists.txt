cmake_minimum_required(VERSION 2.8)

# Project name
project(ADAK_Keying)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 2)
set(PROJECT_VERSION_PATCH 0)

message(STATUS "${CMAKE_SYSTEM_NAME}")

# Build configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if (${CMAKE_SYSTEM_NAME} STREQUAL  "Linux")
    find_package(Protobuf REQUIRED)
else()
    find_package(protobuf REQUIRED)
endif()

# Specific include paths for Mac OS and Windows
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    link_directories(/usr/local/Cellar/protobuf/3.11.4/lib)
    include_directories(/usr/local/include)
endif()

# Specific include paths for Mac OS and Windows
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
endif()

include(CTest)

# Executable
add_subdirectory(src)
install(TARGETS adak DESTINATION ${PROJECT_BINARY_DIR})

# Unit tests
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
