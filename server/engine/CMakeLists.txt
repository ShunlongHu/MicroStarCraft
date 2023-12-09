project(rts_engine)

aux_source_directory(. server_engine)
add_library(${PROJECT_NAME}_shared SHARED ${server_engine})
add_library(${PROJECT_NAME}_static STATIC ${server_engine})
