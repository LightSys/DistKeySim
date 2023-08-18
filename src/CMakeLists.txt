# Compile protobuf files
message(STATUS "Building protobuf from ${CMAKE_CURRENT_SOURCE_DIR}... using ${Protobuf_INCLUDE_DIRS}")
include_directories(${Protobuf_INCLUDE_DIRS} "/usr/local/include")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

add_library(messages message.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(messages ${Protobuf_LIBRARIES})
target_include_directories(messages PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(adakCore
    Damping.cpp
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
    ControlStrategy.cpp
    Factory.cpp
    GeometricDisconnect.cpp
    SystemClock.cpp)

find_package(absl REQUIRED)
find_package(gtest REQUIRED)

target_link_libraries(adakCore messages)
target_include_directories(adakCore PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(adak main.cpp ../include/config.hpp)
target_include_directories(adak PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Overkill with absl:: but it works. How to figure out which ones we don't need?
set(COMMON_LIBRARIES adakCore messages
    absl::atomic_hook absl::errno_saver absl::log_severity absl::raw_logging_internal absl::spinlock_wait absl::config absl::dynamic_annotations absl::core_headers absl::malloc_internal absl::base_internal absl::base absl::throw_delegate absl::pretty_function absl::endian absl::scoped_set_env absl::strerror absl::fast_type_id absl::prefetch absl::algorithm absl::algorithm_container absl::cleanup_internal absl::cleanup absl::btree absl::compressed_tuple absl::fixed_array absl::inlined_vector_internal absl::inlined_vector absl::counting_allocator absl::flat_hash_map absl::flat_hash_set absl::node_hash_map absl::node_hash_set absl::container_memory absl::hash_function_defaults absl::hash_policy_traits absl::common_policy_traits absl::hashtablez_sampler absl::hashtable_debug absl::hashtable_debug_hooks absl::node_slot_policy absl::raw_hash_map absl::container_common absl::raw_hash_set absl::layout absl::crc_cpu_detect absl::crc_internal absl::crc32c absl::non_temporal_arm_intrinsics absl::non_temporal_memcpy absl::crc_cord_state absl::stacktrace absl::symbolize absl::examine_stack absl::failure_signal_handler absl::debugging_internal absl::demangle_internal absl::leak_check absl::debugging absl::flags_path_util absl::flags_program_name absl::flags_config absl::flags_marshalling absl::flags_commandlineflag_internal absl::flags_commandlineflag absl::flags_private_handle_accessor absl::flags_reflection absl::flags_internal absl::flags absl::flags_usage_internal absl::flags_usage absl::flags_parse absl::any_invocable absl::bind_front absl::function_ref absl::hash absl::hash_testing absl::spy_hash_state absl::city absl::low_level_hash absl::log_internal_check_impl absl::log_internal_check_op absl::log_internal_conditions absl::log_internal_config absl::log_internal_flags absl::log_internal_format absl::log_internal_globals absl::log_internal_log_impl absl::log_internal_proto absl::log_internal_message absl::log_internal_log_sink_set absl::log_internal_nullguard absl::log_internal_nullstream absl::log_internal_strip absl::log_internal_voidify absl::log_internal_append_truncated absl::absl_check absl::absl_log absl::check absl::die_if_null absl::log_flags absl::log_globals absl::log_initialize absl::log absl::log_entry absl::log_sink absl::log_sink_registry absl::log_streamer absl::scoped_mock_log absl::log_internal_structured absl::log_structured absl::memory absl::type_traits absl::meta absl::bits absl::int128 absl::numeric absl::numeric_representation absl::sample_recorder absl::exponential_biased absl::periodic_sampler absl::random_random absl::random_bit_gen_ref absl::random_internal_mock_helpers absl::random_distributions absl::random_seed_gen_exception absl::random_seed_sequences absl::random_internal_traits absl::random_internal_distribution_caller absl::random_internal_fast_uniform_bits absl::random_internal_seed_material absl::random_internal_pool_urbg absl::random_internal_salted_seed_seq absl::random_internal_iostream_state_saver absl::random_internal_generate_real absl::random_internal_wide_multiply absl::random_internal_fastmath absl::random_internal_nonsecure_base absl::random_internal_pcg_engine absl::random_internal_randen_engine absl::random_internal_platform absl::random_internal_randen absl::random_internal_randen_slow absl::random_internal_randen_hwaes absl::random_internal_randen_hwaes_impl absl::random_internal_distribution_test_util absl::random_internal_uniform_helper absl::status absl::statusor absl::strings absl::strings_internal absl::str_format absl::str_format_internal absl::cord_internal absl::cordz_update_tracker absl::cordz_functions absl::cordz_statistics absl::cordz_handle absl::cordz_info absl::cordz_sample_token absl::cordz_update_scope absl::cord absl::cord_test_helpers absl::graphcycles_internal absl::kernel_timeout_internal absl::synchronization absl::time absl::civil_time absl::time_zone absl::any absl::bad_any_cast absl::bad_any_cast_impl absl::span absl::optional absl::bad_optional_access absl::bad_variant_access absl::variant absl::compare absl::utility)

# Determine whether we need to add stdc++fs.
# Inspired by https://stackoverflow.com/questions/54290254/problem-adding-stdfilesystem-to-cmake-project
try_compile(HAS_FS "${CMAKE_BINARY_DIR}/temp" 
"${CMAKE_SOURCE_DIR}/has_fs/has_filesystem.cpp" 
            CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
            LINK_LIBRARIES stdc++fs)
if(HAS_FS)
    target_link_libraries(adak ${COMMON_LIBRARIES} stdc++fs)
else()
    target_link_libraries(adak ${COMMON_LIBRARIES})
endif(HAS_FS)

install(TARGETS adak DESTINATION ${PROJECT_BINARY_DIR})
