cmake_minimum_required(VERSION 2.8)

# Project name
project(ADAK_Keying)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

# Build configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#find_package(Protobuf REQUIRED)

# Executable
add_executable(adak
#        src/proto-test.cpp
        src/Simulation.cpp
        src/Node.cpp
        src/Random.cpp
        src/Network.cpp
        src/UUID.cpp
        src/Channel.cpp
#        src/message.pb.cc
)
#target_link_libraries(adak
#        protobuf
#)
target_include_directories(adak
    PUBLIC
        include/
        src/
)
install(TARGETS adak DESTINATION bin)
