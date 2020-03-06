add_library(catch2 tests.cpp)
target_include_directories(catch2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(NodeTests NodeTests.cpp)
target_link_libraries(NodeTests adakCore catch2)

add_test(NAME NodeTest COMMAND NodeTests)