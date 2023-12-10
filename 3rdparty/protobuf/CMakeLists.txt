#find_package(protobuf CONFIG REQUIRED)
#message(STATUS "Using Protocol Buffers ${protobuf_VERSION}")
#if(MSVC AND protobuf_MSVC_STATIC_RUNTIME)
#    foreach(flag_var
#            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
#            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
#        if(${flag_var} MATCHES "/MD")
#            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
#        endif(${flag_var} MATCHES "/MD")
#    endforeach()
#endif()

function(add_proto proto)
    set(proto_bin "D:/repo/vcpkg/packages/protobuf_x64-windows/tools/protobuf/protoc.exe")
    set(grpc_plugin_bin "D:/repo/vcpkg/packages/grpc_x64-windows/tools/grpc/grpc_cpp_plugin.exe")
    set(proto_path ${CMAKE_SOURCE_DIR}/3rdparty/protobuf/include)
    set(proto_gen_path ${CMAKE_SOURCE_DIR}/protos)
    set(proto_out_path_${proto} ${proto_gen_path}/${proto}_gen_cpp)
    file(MAKE_DIRECTORY ${proto_out_path_${proto}})
    add_custom_target(proto_gen_${proto}
            ${proto_bin}
            --plugin=protoc-gen-grpc=${grpc_plugin_bin}
            --proto_path=${proto_gen_path}
            --grpc_out=${proto_out_path_${proto}}
            --cpp_out=${proto_out_path_${proto}}
            ${proto}.proto
    BYPRODUCTS ${proto_gen_path}/${proto}.pb.h ${proto_gen_path}/${proto}.pb.cc
    SOURCES ${proto_gen_path}/${proto}.proto
    )
    include_directories(${proto_out_path_${proto}})
    add_library(${proto}_proto STATIC ${proto_out_path_${proto}}/${proto}.grpc.pb.cc ${proto_out_path_${proto}}/${proto}.pb.cc)
    add_dependencies(${proto}_proto proto_gen_${proto})
    target_link_libraries(${proto}_proto PUBLIC gRPC::grpc++ gRPC::grpc++_reflection gRPC::gpr gRPC::grpc protobuf::libprotobuf)
    message(STATUS "Added Target ${proto_gen_${proto}}.")
endfunction()
