project(rts_cpp_client)
aux_source_directory(. rts_cpp_client)
add_executable(${PROJECT_NAME} ${rts_cpp_client})
target_link_libraries(${PROJECT_NAME} PRIVATE message_proto absl::flags_parse Threads::Threads)
