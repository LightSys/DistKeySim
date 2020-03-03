add_library(adakCore Node.cpp Keyspace.cpp Network.cpp UUID.cpp Channel.cpp Random.cpp message.pb.cc)
target_include_directories(adakCore PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(adak Simulation.cpp)
target_include_directories(adak PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(adak protobuf adakCore)