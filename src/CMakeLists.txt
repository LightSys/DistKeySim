# Compile protobuf files
message(STATUS "Building protobuf from ${CMAKE_CURRENT_SOURCE_DIR}... using ${Protobuf_INCLUDE_DIRS}")
include_directories(${Protobuf_INCLUDE_DIRS} "/usr/local/include")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

add_library(messages message.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(messages ${Protobuf_LIBRARIES})
target_include_directories(messages PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(adakCore
    Simulation.cpp
    Node.cpp
    Keyspace.cpp
    Network.cpp
    UUID.cpp
    Channel.cpp
    Logger.cpp
    Random.cpp
    NodeData.cpp
    config.cpp
    AbstractStrategy.cpp
    ControlStrategy.cpp
    Factory.cpp
    GeometricDisconnect.cpp
    SystemClock.cpp)

target_link_libraries(adakCore messages)
target_include_directories(adakCore PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(adak main.cpp ../include/config.hpp)
target_include_directories(adak PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Determine whether we need to add stdc++fs.
# Inspired by https://stackoverflow.com/questions/54290254/problem-adding-stdfilesystem-to-cmake-project
try_compile(HAS_FS "${CMAKE_BINARY_DIR}/temp" 
"${CMAKE_SOURCE_DIR}/has_fs/has_filesystem.cpp" 
            CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
            LINK_LIBRARIES stdc++fs)
if(HAS_FS)
    target_link_libraries(adak adakCore messages stdc++fs)
else()
    target_link_libraries(adak adakCore messages)
endif(HAS_FS)

install(TARGETS adak DESTINATION ${PROJECT_BINARY_DIR})
