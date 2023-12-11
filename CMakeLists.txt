set(CMAKE_TOOLCHAIN_FILE "D:/repo/vcpkg/scripts/buildsystems/vcpkg.cmake")
cmake_minimum_required(VERSION 3.10)
project(rts)
find_package(absl CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

link_directories("D:/repo/vcpkg/installed/x64-windows/lib"
                "D:/repo/vcpkg/installed/x64-windows/bin")

add_subdirectory(3rdparty)
add_subdirectory(server)
add_subdirectory(observer)