cmake_minimum_required(VERSION 3.20)
project(CellularClusterSDL3)

set(CMAKE_CXX_STANDARD 23)

# Find SDL3
find_package(SDL3 REQUIRED CONFIG)
find_package(OpenCL REQUIRED)

# add main
add_executable(main main.cpp)

# Link SDL3 to the executable
target_link_libraries(main PRIVATE SDL3::SDL3 OpenCL::OpenCL)
