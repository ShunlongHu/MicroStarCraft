project(rts_server)
add_subdirectory(engine)

aux_source_directory(. server_main)
aux_source_directory(service server_main)
include_directories(service)
include_directories(engine)

add_executable(${PROJECT_NAME} ${server_main})
target_link_libraries(${PROJECT_NAME} PRIVATE rts_engine_static message_proto absl::flags_parse Threads::Threads)
