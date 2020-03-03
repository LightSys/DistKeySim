cmake_minimum_required(VERSION 3.15)
project(lightsystemp)

set(CMAKE_CXX_STANDARD 20)

add_executable(lightsystemp src/Simulation.cpp src/Node.cpp src/Random.cpp src/Network.cpp)