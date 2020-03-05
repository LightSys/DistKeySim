add_library(messages message.cpp message.pb.cc)
target_link_libraries(messages protobuf)
target_include_directories(messages PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(adakCore Simulation.cpp Node.cpp Keyspace.cpp Network.cpp UUID.cpp Channel.cpp Random.cpp NodeData.cpp)
target_link_libraries(adakCore messages)
target_include_directories(adakCore PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(adak main.cpp)
target_include_directories(adak PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(adak adakCore messages)
install(TARGETS adak DESTINATION ${PROJECT_BINARY_DIR})
