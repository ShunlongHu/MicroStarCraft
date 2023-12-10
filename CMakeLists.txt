set(CMAKE_TOOLCHAIN_FILE "D:/repo/vcpkg/scripts/buildsystems/vcpkg.cmake")
cmake_minimum_required(VERSION 3.10)
project(rts)
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

add_subdirectory(3rdparty)
add_subdirectory(server)
add_subdirectory(observer)