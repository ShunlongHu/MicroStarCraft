project(rts_cpp_server_starter)
aux_source_directory(. rts_cpp_server_starter)
add_executable(${PROJECT_NAME} ${rts_cpp_server_starter})
target_link_libraries(${PROJECT_NAME} PRIVATE message_proto absl::flags_parse)
