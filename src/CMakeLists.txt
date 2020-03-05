# Compile protobuf files
message(STATUS "Building protobuf from ${CMAKE_CURRENT_SOURCE_DIR}/proto... using ${Protobuf_INCLUDE_DIRS}")
include_directories(${Protobuf_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/proto/message.proto)

add_library(messages message.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(messages ${Protobuf_LIBRARIES})
target_include_directories(messages PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(adakCore Simulation.cpp Node.cpp Keyspace.cpp Network.cpp UUID.cpp Channel.cpp Random.cpp)
target_link_libraries(adakCore messages)
target_include_directories(adakCore PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(adak main.cpp)
target_include_directories(adak PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(adak adakCore messages)
install(TARGETS adak DESTINATION ${PROJECT_BINARY_DIR})
