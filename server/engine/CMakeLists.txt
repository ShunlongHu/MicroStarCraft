project(rts_engine)

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/thread_pool)
aux_source_directory(. server_engine)
add_library(${PROJECT_NAME}_shared SHARED ${server_engine})
add_library(${PROJECT_NAME}_static STATIC ${server_engine})
